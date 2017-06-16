#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "../Engine/InteractiveSurface.h"

namespace OpenXcom
{

enum InversionType {INVERT_NONE, INVERT_CLICK, INVERT_TOGGLE};
	
/**
 * Regular image that works like a button.
 * Unlike the TextButton, this button doesn't draw
 * anything on its own. It takes an existing graphic and
 * treats it as a button, inverting colors when necessary.
 * This is necessary for special buttons like in the Geoscape.
 */
class BattlescapeButton : public InteractiveSurface
{
protected:
	Uint8 _color;
	BattlescapeButton **_group, **_groupSelected;
	bool _inverted, _selected, _excluded;
	InversionType _toggleMode;
	Surface *_altSurface, *_altSurfaceSel, *_altSurfaceInvSel, *_altSurfaceEx;
public:
	/// Creates a new image button with the specified size and position.
	BattlescapeButton(int width, int height, int x = 0, int y = 0);
	/// Cleans up the image button.
	virtual ~BattlescapeButton();
	/// Sets the image button's color.
	void setColor(Uint8 color);
	/// Gets the image button's color.
	Uint8 getColor() const;
	/// Sets the image button's group.
	void setGroup(BattlescapeButton **group);
	/// Sets the image button's group for selected.
	void setGroupSelected(BattlescapeButton **groupSelected);
	/// Special handling for mouse presses.
	void mousePress(Action *action, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(Action *action, State *state);
	/// Invert a button explicitly either ON or OFF.
	void toggle(bool invert);
	/// Toggle selected either ON or OFF, exclusively or not (Extended Reaction Fire).
	void toggleSelected(bool select);
	/// Set the button as excluded or not (Extended Reaction Fire)
	void exclude(bool exclude);
	/// Gets the exclusion status of the button.
	bool getExclusion() const;
	/// Allows this button to be toggled on/off with a click.
	void allowToggleInversion();
	/// Allows this button to be toggled on when clicked, and off when released.
	void allowClickInversion();
	/// Sets up the "pressed" surface.
	void initSurfaces();
	/// Blits this surface onto another one.
	void blit(Surface *surface);
	/// Alters both versions of the button's X pos.
	void setX(int x);
	/// Alters both versions of the button's Y pos.
	void setY(int y);

};

}
