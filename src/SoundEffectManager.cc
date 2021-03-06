/*
 * Copyright (C) 2002,2003,2004 Daniel Heck
 * Copyright (C) 2007,2008 Andreas Lochmann
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "errors.hh"
#include "options.hh"
#include "SoundEffectManager.hh"
#include "SoundEngine.hh"
#include "main.hh"
#include "oxyd.hh"
#include "nls.hh"
#include "client.hh"

#include <string>
#include <cassert>
#include <memory>

using namespace enigma;
using namespace sound;


/* -------------------- Interface Functions -------------------- */

std::string sound::GetOxydSoundSet(OxydLib::OxydVersion oxyd_ver)
{
    return SoundEffectManager::instance()->getOxydSoundSet(oxyd_ver);
}

void sound::InitSoundSets() {
    SoundEffectManager::instance()->initSoundSets();
}

void sound::SetActiveSoundSet(std::string soundset_name)
{
    SoundEffectManager::instance()->setActiveSoundSet(soundset_name);
}

void sound::SetDefaultSoundSet(std::string soundset_name)
{
    SoundEffectManager::instance()->setDefaultSoundSet(soundset_name);
    if(app.state->getString("SoundSetName") == "Default")
        SetActiveSoundSet(soundset_name);
}

/*! The following function is an interface to add sound events to Enigma.
  It is accessed via sound-defaults.lua and user sound definitions. */
void sound::DefineSoundEffect(std::string soundset_key, std::string name, std::string filename,
                              double volume, bool loop, bool global, int priority,
                              double damp_max, double damp_inc, double damp_mult,
                              double damp_min, double damp_tick, std::string silence_string) {
    if(soundset_key == "") {
        Log << "Warning: Tried to define sound event '" << name
            << "' without sound set key. Skipped.\n";
        return;
    }
    SoundEffectManager::instance()->defineSoundEffect(soundset_key, name,
        SoundEffect(name, soundset_key, filename, volume, loop, global, priority,
        damp_max, damp_inc, damp_mult, damp_min, damp_tick, silence_string));
}


bool sound::EmitSoundEvent (const std::string &eventname, const ecl::V2 &pos,
                            double volume, bool force_global)
{
    return SoundEffectManager::instance()->emitSoundEvent(eventname, pos, volume, force_global);
}

bool sound::EmitSoundEventGlobal (const std::string &eventname, double volume)
{
    return SoundEffectManager::instance()->emitSoundEvent(eventname, ecl::V2(), volume, true);
}

void sound::WriteSilenceString (const std::string &eventname)
{
    SoundEffectManager::instance()->writeSilenceString(eventname);
}


/* -------------------- Sound option helpers -------------------- */

/*! These functions are used in OptionsMenu.cc for the Soundset-Button. */

int sound::GetOptionSoundSetCount()
{        
    return SoundEffectManager::instance()->getSoundSetCount() + 1;
}

int sound::GetOptionSoundSet()
{
    std::string soundSet = app.state->getString("SoundSetName");
    if (soundSet == "Default")
        return 0;
    int pos = SoundEffectManager::instance()->getSoundSetButtonPosition(soundSet);
    assert(pos > 0);
    return pos;
}

void sound::SetOptionSoundSet(int value)
{
    if(value == 0) {
        // setting to default sound set
        if (app.state->getString("SoundSetName") == "Default")
            return;
        app.state->setProperty("SoundSetName", "Default");
        options::SetOption("SoundSet", SoundEffectManager::instance()->convertToOldSoundSetNumber("Default"));
        SetActiveSoundSet(SoundEffectManager::instance()->getDefaultSoundSet());
    } else {
        std::string newSet = SoundEffectManager::instance()->getSoundSetByPosition(value);
        assert(newSet != "");
        if (app.state->getString("SoundSetName") == newSet)
            return;
        app.state->setProperty("SoundSetName", newSet);
        options::SetOption("SoundSet", SoundEffectManager::instance()->convertToOldSoundSetNumber(newSet));
        SetActiveSoundSet(newSet);
    }
}

std::string sound::GetOptionSoundSetText(int value)
{
    if(value == 0)
        return N_("Default");
    std::string soundset_name = SoundEffectManager::instance()->getSoundSetByPosition(value);
    if(soundset_name == "")
        return "INVALID";
    return soundset_name;
}

/* -------------------- SoundEvent implementation -------------------- */

SoundEvent::SoundEvent ()
: name(""), has_position(false), position(),
  priority(0), volume(0.0), range(30.0), fullvol_range(0.2),
  number_events(1), left(0), right(0), active(false),
  playing_time(0)
{}

double SoundEvent::effectiveVolume(double dist) {
    double reduced_dist = std::max(0.0, dist - fullvol_range);
    return (1 - reduced_dist / range) * volume;
}

bool SoundEvent::merge(SoundEvent se) {
    if (has_position && se.has_position) {
        /*! Merge the soundevent se onto this one. The loudness profile
           of a soundevent se is that of a cylinder shaped center region
           with height se.volume and radius se.fullvol_range,
           surrounded by a linearly decreasing area of breadth se.range.
           The merged soundevent will have a center region covering both
           old center regions, and its volume is set to the maximum
           of the two volumes. */
        double dist = ecl::length(position - se.position);
        volume = std::max(volume, se.volume);
        fullvol_range += dist/2;
        position = (number_events * position + se.number_events * se.position)
                       / (number_events + se.number_events);
        number_events += se.number_events;
        return true;
    } else {
        // Don't merge global sounds.
        return false;
    }
}            

/* -------------------- SoundEffectManager -------------------- */

SoundEffectManager *SoundEffectManager::theSingleton = 0;
    
SoundEffectManager* SoundEffectManager::instance() {
    if (theSingleton == 0) {
        theSingleton = new SoundEffectManager();
    }
    return theSingleton;
}

SoundEffectManager::SoundEffectManager()
: sound_sets(), sound_effects(), active_sound_set_key(""),
  default_sound_set(""), sound_set_count(0)
{}    

/* ------------- Conversion and helper functions ------------- */

/*! This function defines how to put soundset_key and eventname together to
  create an eventkey. */

std::string SoundEffectManager::effectKey(std::string effect_name, std::string soundset_name)
{
    if (soundset_name == "")
        return getActiveSoundSetKey() + "#" + effect_name;
    else
        return soundset_name + "#" + effect_name;
}

/*! This function searches the known sound sets for a sound set with given
  oxyd version, and returns the sound set name (or empty string if none). */

std::string SoundEffectManager::getOxydSoundSet(OxydLib::OxydVersion oxyd_ver)
{
    for (SoundSetRepository::iterator i = sound_sets.begin();
             i != sound_sets.end(); ++i)
        if((*i).second.getOxydVersion() == oxyd_ver)
            return (*i).first;
    return "";
}

/*! Enigma 1.00 only knew the option "SoundSet", which was an integer. This
  was quite unhandy if one wanted to add additional sound sets. Since 1.01
  Enigma features the option "SoundSetName". Still, old "SoundSet" is needed
  if user wants to switch to <= 1.00 again; so here are the conversion
  functions. Any user sound set is mapped to 0 ("Default").  */

int SoundEffectManager::convertToOldSoundSetNumber(std::string soundset_name)
{
    if(soundset_name == "Default")  return 0;
    if(soundset_name == "Enigma")   return 1;
    SoundSet sd = sound_sets[soundset_name];
    if(sd.isOxyd())
        return ((int) sd.getOxydVersion()) + 2;
    return 0;
}

std::string SoundEffectManager::convertFromOldSoundSetNumber(int soundset_number)
{
    if(soundset_number == 0)  return "Default";
    if(soundset_number == 1)  return "Enigma";
    return getOxydSoundSet((OxydLib::OxydVersion) (soundset_number - 2));
}

/* -------------------- Sound set handling -------------------- */

/*! These functions fill in data for the sound sets, initialises and
  activates them. Return false, if something went wrong, e.g. when an
  oxyd sound set is mentioned, but the corresponding oxyd version
  wasn't found. */

bool SoundEffectManager::defineSoundSet(std::string soundset_name, std::string soundset_key,
                                 int button_position)
{
    sound_sets[soundset_name] = SoundSet(soundset_key, button_position);
    Log << "Added sound set '" << soundset_name << "' (key '" << soundset_key
        << "') on position " << button_position << ".\n";
    return true;
}

bool SoundEffectManager::defineSoundSetOxyd(std::string soundset_name, std::string soundset_key,
                                     OxydLib::OxydVersion oxyd_ver, int button_position)
{
    if(oxyd::FoundOxyd(oxyd_ver)) {
        sound_sets[soundset_name] =
            SoundSet(soundset_key, button_position, (OxydLib::OxydVersion) oxyd_ver);
        Log << "Added sound set '" << soundset_name << "' (key '" << soundset_key
            << "') on position " << button_position << ".\n";
        return true;
    } else {
        Log << "Skipped sound set '" << soundset_name << "'.\n";
        return false;
    }
}

bool SoundSet::activate()
{
    if(getSoundSetKey() == "")
        return false;
    if(isOxyd() &&  !oxyd::InitOxydSoundSet(getOxydVersion()))
        return false;

    SoundEffectManager::instance()->setActiveSoundSetKey(getSoundSetKey());
    return true;
}

void SoundEffectManager::initSoundSets()
{
    // Define sound sets
    sound_sets.clear();
    assert(sound_sets.empty());
#if defined(__vita__) || defined(__SWITCH__)
    defineSoundSet ("Enigma",   "Enigma",  1);
#else
    assert(defineSoundSet ("Enigma",   "Enigma",  1));
#endif
    int pos = 2; // position in options menu button
    if (defineSoundSetOxyd ("Oxyd",     "Oxyd*",   OxydLib::OxydVersion_Oxyd1,          pos))  pos++;
    if (defineSoundSetOxyd ("Magnum",   "Magnum*", OxydLib::OxydVersion_OxydMagnum,     pos))  pos++;
    if (defineSoundSetOxyd ("Mag.Gold", "Magnum*", OxydLib::OxydVersion_OxydMagnumGold, pos))  pos++;
    if (defineSoundSetOxyd ("Per.Oxyd", "Oxyd*",   OxydLib::OxydVersion_PerOxyd,        pos))  pos++;
    if (defineSoundSetOxyd ("Extra",    "Oxyd*",   OxydLib::OxydVersion_OxydExtra,      pos))  pos++;
    // Define user sound sets, as given by sound_effects
    for (SoundEffectRepository::iterator i = sound_effects.begin();
         i != sound_effects.end(); ++i) {
        std::string soundset_key = (*i).second.getSoundSetKey();
        bool found = false;
        // ignore Oxyd* and Magnum* sound effects, if no 
        if ((soundset_key != "Oxyd*") && (soundset_key != "Magnum*")) {
            for (SoundSetRepository::iterator j = sound_sets.begin();
                 j != sound_sets.end(); ++j)
                if((*j).second.getSoundSetKey() == soundset_key)
                    found = true;
            if(!found)
                if (defineSoundSet (soundset_key, soundset_key, pos))  pos++;
        }
    }
    Log << "Found " << pos - 1 << " different sound sets.\n";
    setSoundSetCount(pos - 1);
    setDefaultSoundSet("Enigma");
    // Extract sound set names and keys from options; activate!
    std::string soundset_name = app.state->getString("SoundSetName");
    if (soundset_name == "") { // just switched from 1.00 to higher
        soundset_name = convertFromOldSoundSetNumber(options::GetInt("SoundSet"));
        app.state->setProperty("SoundSetName", soundset_name);
    }
    if (soundset_name == "Default")
        soundset_name = getDefaultSoundSet();
    ClearCache();
    if (sound_sets[soundset_name].activate())  {
        preloadSoundEffects();
        Log << "Activated sound set '" << soundset_name << "'.\n";
    }
    else {
        // Fallback, happens e.g. when oxyd sound set can't be established or 
        // a user soundset is given which doesn't exist anymore.
        Log << "Warning: Soundset '" << soundset_name << "' not available.\n";
        if (sound_sets["Enigma"].activate()) {
            app.state->setProperty("SoundSetName", "Enigma");
            options::SetOption("SoundSet", convertToOldSoundSetNumber("Enigma"));
        } else
            ASSERT(false, XFrontend,
                "Soundsets defect and fallback 'Enigma' not available.");
    }
}

void SoundEffectManager::setActiveSoundSet(std::string soundset_name)
{
    std::string soundset_key = sound_sets[soundset_name].getSoundSetKey();
    if (soundset_key == getActiveSoundSetKey())
        return;
    if (soundset_key == "Default") {
        Log << "Warning: Tried to choose 'Default' as effective sound set.\n";
        return;
    }
    if (soundset_key == "") {
        Log << "Warning: Tried to choose empty sound set key as effective sound set.\n";
        return;
    }
    ClearCache();
    if (sound_sets[soundset_name].activate()) {
        preloadSoundEffects();
        Log << "Switched to sound set '" << soundset_name << "' (key '" 
            << soundset_key << "').\n";
    } else
        Log << "Warning: Problems loading sound set '" << soundset_name << "' (key'"
            << soundset_key << "').\n";
}

/*! Pre-cache all sound effects so that they can be played without lag
  when the game is running. */
void SoundEffectManager::preloadSoundEffects()
{
    SoundEffectRepository::iterator i = sound_effects.begin(), 
        end = sound_effects.end();
    std::string prefix = getActiveSoundSetKey() + "#";
    for (; i != end; ++i) {
        if (i->first.compare(0, prefix.size(), prefix) == 0)
            CacheSound(i->second);
    }
}

std::string SoundEffectManager::getSoundSetByPosition(int button_position)
{
    for (SoundSetRepository::iterator i = sound_sets.begin();
             i != sound_sets.end(); ++i)
        if((*i).second.getButtonPosition() == button_position)
            return (*i).first;
    return "";
}

/* -------------------- Playing sound events -------------------- */

/*! The following method is the interface between the formal SoundEffect
  and the sound engine (via PlaySound[Global]). The last two functions
  define the interface between level objects and SoundEffect. */

bool SoundEffect::play(const ecl::V2 &pos, double vol, bool glob)
{
    if (name == "")
        return false;
    if (filename == "") {
        Log << "No soundfile given for sound event " << name << ".\n";
        return false;
    }
    if (glob || global)
        return PlaySoundGlobal (filename, volume * vol, priority);
    else
        return PlaySound (filename, pos, volume * vol, priority);
}

bool SoundEffectManager::emitSoundEvent (const std::string &eventname, const ecl::V2 &pos,
                            double volume, bool force_global)
{
    if((volume == 0) || sound::IsSoundMute())
        return false;
    std::string effectkey = effectKey(eventname);
    SoundEffectRepository::iterator i = sound_effects.find(effectkey);
    if (i == sound_effects.end()) {
        Log << "Undefined sound event " << effectkey << " @ " 
            << pos[0] << "," << pos[1] << "\n";
        return false;
    } else {
        return i->second.play(pos, volume, force_global);
    }
}

void SoundEffectManager::writeSilenceString (const std::string &eventname)
{
    std::string effectkey = effectKey(eventname);
    SoundEffectRepository::iterator i = sound_effects.find(effectkey);
    if (i != sound_effects.end()) {
        std::string silence_string = (*i).second.getSilenceString();
        if (silence_string != "")
            client::Msg_ShowText (silence_string, true);
    }
}

/* -------------------- Sound damping implementation -------------------- */

/*! These methods are connected to the sound damping mechanism, designed
  to reduce the noise created by some objects like st_lightpassenger. */

SoundDamping::SoundDamping(std::string effect_name_, const void *origin_)
: effect_name(effect_name_), origin(origin_)
{
    damp = SoundEffectManager::instance()->getDampingData(effect_name_);
    factor = damp.incr;
    //Log << "New damping entry " << effect_name << " with " << damp.incr << ".\n";
}

float SoundDamping::get_volume(float def_volume) {
    if (factor < damp.maxi)
        factor += damp.incr;
    //Log << "  Found entry " << effect_name << ". Factor is now " << i->factor << ".\n";
    float q = factor * damp.mult;
    if (q > 1.0)
        return def_volume / q;
    return def_volume;
}

bool SoundDamping::tick() {
    // return true, if this entity is to be destroyed.
    factor *= damp.tick;
    return (factor <= damp.mini);
}
