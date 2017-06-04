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
#include "BattlescapeButton.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Sets up a battlescape button with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
BattlescapeButton::BattlescapeButton(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _color(0), _group(0), _groupSelected(0), _inverted(false), _selected(false), _toggleMode(INVERT_NONE), _altSurface(0), _altSurfaceSel(0), _altSurfaceInvSel(0)
{
}

/**
 *
 */
BattlescapeButton::~BattlescapeButton()
{
	delete _altSurface;
	delete _altSurfaceSel;
	delete _altSurfaceInvSel;
}

/**
 * Changes the color for the battlescape button.
 * @param color Color value.
 */
void BattlescapeButton::setColor(Uint8 color)
{
	_color = color;
}

/**
 * Returns the color for the battlescape button.
 * @return Color value.
 */
Uint8 BattlescapeButton::getColor() const
{
	return _color;
}

/**
 * Changes the button group this battlescape button belongs to, for general TU reservation.
 * @param group Pointer to the pressed button pointer in the group.
 * Null makes it a regular button.
 */
void BattlescapeButton::setGroup(BattlescapeButton **group)
{
	_group = group;
	if (_group != 0 && *_group == this)
		_inverted = true;
}
	
/**
 * Changes the button group this battlescape button belongs to, for individual TU reservation.
 * @param group Pointer to the pressed button pointer in the group.
 * Null makes it a regular button.
 */
void BattlescapeButton::setGroupSelected(BattlescapeButton **groupSelected)
{
	_groupSelected = groupSelected;
	if (_groupSelected != 0 && *_groupSelected == this)
		_selected = true;
}

/**
 * Sets the button as the pressed button if it's part of a group,
 * and inverts the colors when pressed.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void BattlescapeButton::mousePress(Action *action, State *state)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		if (_group != 0)
		{
			(*_group)->toggle(false);
			*_group = this;
			_inverted = true;
		}
		else if ((_tftdMode || _toggleMode == INVERT_CLICK ) && !_inverted && isButtonPressed() && isButtonHandled(action->getDetails()->button.button))
		{
			_inverted = true;
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		if (_groupSelected != 0)
		{
			(*_groupSelected)->toggleSelected(false);
			*_groupSelected = this;
			Options::extendedReactionFire ? _selected = true : _selected = false;
		}
		else if ((_tftdMode || _toggleMode == INVERT_CLICK ) && !_selected && isButtonPressed() && isButtonHandled(action->getDetails()->button.button))
		{
			Options::extendedReactionFire ? _selected = true : _selected = false;
		}
	}
	InteractiveSurface::mousePress(action, state);
}

/*
 * Sets the button as the released button if it's part of a group.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void BattlescapeButton::mouseRelease(Action *action, State *state)
{
	if (_inverted && isButtonHandled(action->getDetails()->button.button))
	{
		if (_group == 0) // For zero TUs button
		{
			_inverted = false;
			_selected = false;
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_LEFT) // Left clicked on a reserve button
		{
			_inverted = false;
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) // Right clicked on a reserve button
		{
			_selected = false;
		}
	}
	InteractiveSurface::mouseRelease(action, state);
}

/**
 * Invert a button explicitly either ON or OFF and keep track of the state using our internal variables.
 * @param press Set this button as pressed.
 */
void BattlescapeButton::toggle(bool press)
{
	if (_tftdMode || _toggleMode == INVERT_TOGGLE || _inverted)
	{
		_inverted = press;
	}
}
	
/**
 * Toggling reserve TU button for individual selection either ON or OFF and keep track of the state using our internal variables.
 * @param press Set this button as pressed.
 */
void BattlescapeButton::toggleSelected(bool press)
{
	Options::extendedReactionFire ? _selected = press : _selected = false;
}
	
/**
 * Toggle inversion mode: click to press, click to unpress.
 */
void BattlescapeButton::allowToggleInversion()
{
	_toggleMode = INVERT_TOGGLE;
}

/**
 * Click inversion mode: click to press, release to unpress.
 */
void BattlescapeButton::allowClickInversion()
{
	_toggleMode = INVERT_CLICK;
}

/**
 * Initializes the alternate surface for swapping out as needed.
 * performs a colour swap for TFTD style buttons, and a palette inversion for coloured buttons
 * we use two separate surfaces because it's far easier to keep track of
 * whether or not this surface is inverted.
 */
void BattlescapeButton::initSurfaces()
{
	delete _altSurface;
	_altSurface = new Surface(_surface->w, _surface->h, _x, _y); // For inverted reserve button
	_altSurface->setPalette(getPalette());
	delete _altSurfaceSel;
	_altSurfaceSel = new Surface(_surface->w, _surface->h, _x, _y); // For selected but not inverted reserve button
	_altSurfaceSel->setPalette(getPalette());
	delete _altSurfaceInvSel;
	_altSurfaceInvSel = new Surface(_surface->w, _surface->h, _x, _y); // For selected and inverted reserve button
	_altSurfaceInvSel->setPalette(getPalette());
	
	// Lock the surface
	_altSurface->lock();
	_altSurfaceSel->lock();
	_altSurfaceInvSel->lock();

	// FIXME: IMPLEMENT OTHER ALTSURFACES FOR TFTD MODE
	// tftd mode: use a colour lookup table instead of simple palette inversion for our "pressed" state
	if (_tftdMode)
	{
		// this is our colour lookup table
		const int colorFrom[] = {1, 2, 3, 4,  7,  8, 31, 47, 153, 156, 159};
		const int colorTo[]   = {2, 3, 4, 5, 11, 10,  2,  2,  96,   9,  97};

		for (int x = 0, y = 0; x < getWidth() && y < getHeight();)
		{
			Uint8 pixel = getPixel(x, y);
			for (int i = 0; i != sizeof(colorFrom)/sizeof(colorFrom[0]); ++i)
			{
				if (pixel == colorFrom[i])
				{
					pixel = colorTo[i];
					break;
				}
			}
			_altSurface->setPixelIterative(&x, &y, pixel);
		}
	}
	else
	{
		for (int x = 0, y = 0; x < getWidth() && y < getHeight();)
		{
			Uint8 pixel = getPixel(x, y);
			if (pixel > 0)
			{
				_altSurface->setPixelIterative(&x, &y, pixel + 2 * ((int)_color + 3 - (int)pixel));
			}
			else
			{
				_altSurface->setPixelIterative(&x, &y, 0);
			}
		}
		for (int x = 0, y = 0; x < getWidth() && y < getHeight();)
		{
			Uint8 pixel = getPixel(x, y);
			if (pixel == 34) // The colour of the shooting figure in reserve button
			{
				_altSurfaceSel->setPixelIterative(&x, &y, pixel + 2 * ((int)_color - 10 - (int)pixel));
			}
			else
			{
				_altSurfaceSel->setPixelIterative(&x, &y, 0);
			}
		}
		for (int x = 0, y = 0; x < getWidth() && y < getHeight();)
		{
			Uint8 pixel = getPixel(x, y);
			if (pixel > 0)
			{
				if (pixel == 34) {
					_altSurfaceInvSel->setPixelIterative(&x, &y, pixel + 2 * ((int)_color - 10 - (int)pixel));
				}
				else
				{
					_altSurfaceInvSel->setPixelIterative(&x, &y, pixel + 2 * ((int)_color + 3 - (int)pixel));
				}
			}
			else
			{
				_altSurfaceInvSel->setPixelIterative(&x, &y, 0);
			}
		}
	}
	
	// Unlock the surface
	_altSurface->unlock();
	_altSurfaceSel->unlock();
	_altSurfaceInvSel->unlock();
}

/**
 * Blits this surface or the alternate surface onto another one,
 * depending on whether the button is "pressed" or not.
 * @param surface Pointer to surface to blit onto.
 */
void BattlescapeButton::blit(Surface *surface)
{
	if (_inverted && !_selected)
	{
		_altSurface->blit(surface);
	}
	else if (!_inverted && _selected)
	{
		_altSurfaceSel->blit(surface);
	}
	else if (_inverted && _selected)
	{
		_altSurfaceInvSel->blit(surface);
	}
	else
	{
		Surface::blit(surface);
	}
}

/**
 * Changes the position of the surface in the X axis.
 * @param x X position in pixels.
 */
void BattlescapeButton::setX(int x)
{
	Surface::setX(x);
	if (_altSurface)
	{
		_altSurface->setX(x);
	}
}

/**
 * Changes the position of the surface in the Y axis.
 * @param y Y position in pixels.
 */
void BattlescapeButton::setY(int y)
{
	Surface::setY(y);
	if (_altSurface)
	{
		_altSurface->setY(y);
	}
}
}
