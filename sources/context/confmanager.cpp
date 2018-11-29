/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "confmanager.h"
#include <QFileInfo>
#include <QDir>

ConfManager::ConfManager(): QObject(),
    _settings(this)
{
    // Possibly reset the configuration if the previous version was < 2.0
    if (this->getValue(SECTION_NONE, "last_version_installed", 0.).toDouble() < 2.0)
        this->clear();
    this->setValue(SECTION_NONE, "last_version_installed", VERSION);
}

QVariant ConfManager::getValue(Section section, QString key, QVariant defaultValue) const
{
    return _settings.value(getFullKey(section, key), defaultValue);
}

QVariant ConfManager::getToolValue(ToolType toolType, QString toolName, QString key, QVariant defaultValue) const
{
    return _settings.value(getFullKey(toolType, toolName, key), defaultValue);
}

void ConfManager::setValue(Section section, QString key, QVariant value)
{
    _settings.setValue(getFullKey(section, key), value);

    // Possibly update elements
    if (section == Section::SECTION_SOUND_ENGINE)
        emit(soundEngineConfigurationChanged());
    else if (section == Section::SECTION_AUDIO)
    {
        emit(soundEngineConfigurationChanged()); // First prepare the sound engine (the buffer can be adjusted)
        emit(audioServerConfigurationChanged()); // Then update the audio server configuration
    }
    else if (section == Section::SECTION_MAP)
        emit(keyMapChanged());
    else if (section == Section::SECTION_NONE && key == "name_middle_c")
        emit(interfaceChanged());
}

void ConfManager::setToolValue(ToolType toolType, QString toolName, QString key, QVariant value)
{
    _settings.setValue(getFullKey(toolType, toolName, key), value);
}

QString ConfManager::getFullKey(Section section, QString key) const
{
    // First part depends on the section
    QString firstPart = "";
    switch (section)
    {
    case SECTION_NONE:         firstPart = "";            break;
    case SECTION_DISPLAY:      firstPart = "affichage";   break;
    case SECTION_AUDIO:        firstPart = "audio";       break;
    case SECTION_BULK_RENAME:  firstPart = "bulk_rename"; break;
    case SECTION_COLORS:       firstPart = "colors";      break;
    case SECTION_EXPORT:       firstPart = "export";      break;
    case SECTION_KEYBOARD:     firstPart = "keyboard";    break;
    case SECTION_MAP:          firstPart = "map";         break;
    case SECTION_MIDI:         firstPart = "midi";        break;
    case SECTION_RECENT_FILES: firstPart = "recent_file"; break;
    case SECTION_SOUND_ENGINE: firstPart = "synth";       break;
    case SECTION_TOOLS:        firstPart = "tools";       break;
    case SECTION_WARNINGS:     firstPart = "warnings";    break;
    case SECTION_REPOSITORY:   firstPart = "repository";  break;
    }
    if (firstPart != "")
        firstPart += "/";

    return firstPart + key;
}

QString ConfManager::getFullKey(ToolType toolType, QString toolName, QString key) const
{
    QString subSection = "";
    switch (toolType)
    {
    case TOOL_TYPE_SF2:        subSection = "sf2";        break;
    case TOOL_TYPE_SAMPLE:     subSection = "sample";     break;
    case TOOL_TYPE_INSTRUMENT: subSection = "instrument"; break;
    case TOOL_TYPE_PRESET:     subSection = "preset";     break;
    }

    return getFullKey(SECTION_TOOLS, subSection + "/" + toolName + "_" + key);
}

QString ConfManager::getConfigDir()
{
    return QFileInfo(_settings.fileName()).dir().path();
}

QString ConfManager::getMapping(int numOctave, Key key)
{
    QString txt = "";

    // Default value
    switch (numOctave)
    {
    case 0:
        switch (key)
        {
        case KEY_C_LEFT:  txt = trUtf8("Z", "first key mapping for bottom left C"); break;
        case KEY_C_SHARP: txt = trUtf8("S", "first key mapping for bottom C#"); break;
        case KEY_D:       txt = trUtf8("X", "first key mapping for bottom D"); break;
        case KEY_D_SHARP: txt = trUtf8("D", "first key mapping for bottom D#"); break;
        case KEY_E:       txt = trUtf8("C", "first key mapping for bottom E"); break;
        case KEY_F:       txt = trUtf8("V", "first key mapping for bottom F"); break;
        case KEY_F_SHARP: txt = trUtf8("G", "first key mapping for bottom F#"); break;
        case KEY_G:       txt = trUtf8("B", "first key mapping for bottom G"); break;
        case KEY_G_SHARP: txt = trUtf8("H", "first key mapping for bottom G#"); break;
        case KEY_A:       txt = trUtf8("N", "first key mapping for bottom A"); break;
        case KEY_A_SHARP: txt = trUtf8("J", "first key mapping for bottom A#"); break;
        case KEY_B:       txt = trUtf8("M", "first key mapping for bottom B"); break;
        case KEY_C_RIGHT: txt = trUtf8(",", "first key mapping for bottom right C"); break;
        }
        break;
    case 1:
        switch (key)
        {
        case KEY_C_LEFT:  txt = trUtf8("Q", "first key mapping for upper left C"); break;
        case KEY_C_SHARP: txt = trUtf8("2", "first key mapping for upper C#"); break;
        case KEY_D:       txt = trUtf8("W", "first key mapping for upper D"); break;
        case KEY_D_SHARP: txt = trUtf8("3", "first key mapping for upper D#"); break;
        case KEY_E:       txt = trUtf8("E", "first key mapping for upper E"); break;
        case KEY_F:       txt = trUtf8("R", "first key mapping for upper F"); break;
        case KEY_F_SHARP: txt = trUtf8("5", "first key mapping for upper F#"); break;
        case KEY_G:       txt = trUtf8("T", "first key mapping for upper G"); break;
        case KEY_G_SHARP: txt = trUtf8("6", "first key mapping for upper G#"); break;
        case KEY_A:       txt = trUtf8("Y", "first key mapping for upper A"); break;
        case KEY_A_SHARP: txt = trUtf8("7", "first key mapping for upper A#"); break;
        case KEY_B:       txt = trUtf8("U", "first key mapping for upper B"); break;
        case KEY_C_RIGHT: txt = trUtf8("I", "first key mapping for upper right C"); break;
        }
        break;
    case 2:
        switch (key)
        {
        case KEY_C_LEFT:  txt = trUtf8("Shift+Z", "second key mapping for bottom left C"); break;
        case KEY_C_SHARP: txt = trUtf8("Shift+S", "second key mapping for bottom C#"); break;
        case KEY_D:       txt = trUtf8("Shift+X", "second key mapping for bottom D"); break;
        case KEY_D_SHARP: txt = trUtf8("Shift+D", "second key mapping for bottom D#"); break;
        case KEY_E:       txt = trUtf8("Shift+C", "second key mapping for bottom E"); break;
        case KEY_F:       txt = trUtf8("Shift+V", "second key mapping for bottom F"); break;
        case KEY_F_SHARP: txt = trUtf8("Shift+G", "second key mapping for bottom F#"); break;
        case KEY_G:       txt = trUtf8("Shift+B", "second key mapping for bottom G"); break;
        case KEY_G_SHARP: txt = trUtf8("Shift+H", "second key mapping for bottom G#"); break;
        case KEY_A:       txt = trUtf8("Shift+N", "second key mapping for bottom A"); break;
        case KEY_A_SHARP: txt = trUtf8("Shift+J", "second key mapping for bottom A#"); break;
        case KEY_B:       txt = trUtf8("Shift+M", "second key mapping for bottom B"); break;
        case KEY_C_RIGHT: txt = trUtf8("Shift+<", "second key mapping for bottom right C"); break;
        }
        break;
    case 3:
        switch (key)
        {
        case KEY_C_LEFT:  txt = trUtf8("Shift+Q", "second key mapping for upper left C"); break;
        case KEY_C_SHARP: txt = trUtf8("Shift+@", "second key mapping for upper C#"); break;
        case KEY_D:       txt = trUtf8("Shift+W", "second key mapping for upper D"); break;
        case KEY_D_SHARP: txt = trUtf8("Shift+#", "second key mapping for upper D#"); break;
        case KEY_E:       txt = trUtf8("Shift+E", "second key mapping for upper E"); break;
        case KEY_F:       txt = trUtf8("Shift+R", "second key mapping for upper F"); break;
        case KEY_F_SHARP: txt = trUtf8("Shift+%", "second key mapping for upper F#"); break;
        case KEY_G:       txt = trUtf8("Shift+T", "second key mapping for upper G"); break;
        case KEY_G_SHARP: txt = trUtf8("Shift+^", "second key mapping for upper G#"); break;
        case KEY_A:       txt = trUtf8("Shift+Y", "second key mapping for upper A"); break;
        case KEY_A_SHARP: txt = trUtf8("Shift+&", "second key mapping for upper A#"); break;
        case KEY_B:       txt = trUtf8("Shift+U", "second key mapping for upper B"); break;
        case KEY_C_RIGHT: txt = trUtf8("Shift+I", "second key mapping for upper right C"); break;
        }
        break;
    default:
        break;
    }

    // The default value is possibly overriden
    return this->getValue(SECTION_MAP, "key_" + QString::number(numOctave) + "_" + QString::number((int)key), txt).toString();
}

void ConfManager::clear()
{
    _settings.clear();
}
