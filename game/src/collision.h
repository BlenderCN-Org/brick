/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * sdl_game_test
 * Copyright (C) Chandra 2013 <chandra@Laptop>
 * 
sdl_game_test is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * sdl_game_test is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _COLLISION_H_
#define _COLLISION_H_

namespace Collision {
	bool aa_rect(const float *r1_pos,
             	 const float *r1_dim,
                 const float *r2_pos,
                 const float *r2_dim);
}

#endif // _COLLISION_H_
