/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "sf2pdtapart_mod.h"
#include <QDataStream>

Sf2PdtaPart_mod::Sf2PdtaPart_mod() :
    _isValid(false),
    _sfModDestOper(0),
    _modAmount(0),
    _sfModTransOper(0)
{
    _sfModSrcOper.CC = 0;
    _sfModSrcOper.D = 0;
    _sfModSrcOper.Index = 0;
    _sfModSrcOper.P = 0;
    _sfModSrcOper.Type = 0;

    _sfModAmtSrcOper.CC = 0;
    _sfModAmtSrcOper.D = 0;
    _sfModAmtSrcOper.Index = 0;
    _sfModAmtSrcOper.P = 0;
    _sfModAmtSrcOper.Type = 0;
}

QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_mod &mod)
{
    in >> mod._sfModSrcOper >> mod._sfModDestOper >> mod._modAmount >> mod._sfModAmtSrcOper >> mod._sfModTransOper;

    mod._isValid = true;
    return  in;
}
