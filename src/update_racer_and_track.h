







void update_racer_and_track(Racer *r, Track *track) // includes masking
{

	static int every_other = 0;
	every_other++;

	//if ((every_other%2) != 0) return;

	static float turn_direction_velocity = 0;
	//if (r->throttle_on) r->velocity_magnitude += 0.065; // good


	if (r->throttle_on) r->velocity_magnitude += 0.085;
	if (r->break_on) r->velocity_magnitude *= 0.9;
	if (r->turning_right) turn_direction_velocity -= 0.014; // good
	if (r->turning_left) turn_direction_velocity += 0.014; // good

	turn_direction_velocity = (turn_direction_velocity * 0.8); // good

	r->direction_angle += turn_direction_velocity * OMG_DeltaTime;

	r->velocity_magnitude *= (1.0-abs(turn_direction_velocity*0.05));// = (turn_direction_velocity * 0.8); // good
	
	// friction
	//r->velocity_magnitude *= 0.98;
	r->velocity_magnitude *= 0.98; // good



	r->direction = vec2d(sin(r->direction_angle), cos(r->direction_angle));
	r->velocity = (r->velocity*0.6 + r->direction*0.4).Normalized() * r->velocity_magnitude;



	Racer &player = *r;




	int __HACK_entrance_already_collided = 0;



	//if (!index_of_last_terrain_that_collides) last_terrain_that_collides = track->segment[0];



	//player.velocity_y += GRAVITY;
	//player.update_walking_velocity();
	//if (!player.moving_left && !player.moving_right) player.velocity_x *= 0.5;
	//player.on_ground = false;

	vec2d start_point = vec2d(player.position.x, player.position.y);
	vec2d end_point = start_point + vec2d(player.velocity.x, player.velocity.y) * player.velocity_magnitude * OMG_DeltaTime;
	

	vec2d player_pos = start_point;
	vec2d player_vel = end_point - start_point;
	vec2d player_end_pos = end_point;


	//player.position = end_point;
	//return;



	float time_left_in_timestep = 1.0;
	int segment_that_collides = NULL;
	//Terrain *terrain_that_collides = NULL;
	TrackSegment *terrain_that_collides = NULL;
	bool collides_on_left_terrain = false;
	vec2d point_of_intersection;

	int num_steps = 0;

	bool collides_through_exit = false;
	
	bool collides_through_entrance = false;
	int entrance_that_collides = 0;

	//if(debug) draw_crosshair(player_pos, al_color_name("green"));

	int terrain_segment_where_player_collides = 0;

	while(time_left_in_timestep > 0)
	{
		entrance_that_collides = 0;
		//
		// find the soonest intersection time
		//

		float collision_time = 1.0;

		SegmentInfo motion_segment(player_pos, player_pos+player_vel*time_left_in_timestep);

		vec2d &E = motion_segment.from_start;//*terrain.point[i] - *terrain.point[i-1];
		vec2d &P1 = motion_segment.perpendicular;//Vec2d(-F.y, F.x);
		terrain_that_collides = NULL;
		collides_through_exit = false;
		collides_through_entrance = false;

		//for (int t=0; t<(int)map.terrain.size(); t++)
		int track_segment_start = max(index_of_last_track_segment_that_collides-2, 0);
		int track_segment_end = min(index_of_last_track_segment_that_collides+2, (int)track->segment.size()-1);

		//std::cout << "collision start end: " << track_segment_start << " -- " << track_segment_end << std::endl;

		for (int t=track_segment_start; t<(int)(track_segment_end+1); t++)
		{
			// just the left rails first

			//Terrain *terrain = map.terrain[t];
			TrackSegment *terrain = track->segment[t];

			
			//for (int i=1; i<(int)terrain->point.size(); i++)
			for (int i=1; i<(int)terrain->left_rail.size(); i++)
			{
				vec2d &F = terrain->left_rail_segment[i]->from_start;//*terrain.point[i] - *terrain.point[i-1];
				vec2d &P2 = terrain->left_rail_segment[i]->perpendicular;//Vec2d(-F.y, F.x);

				float h = ((motion_segment.start - terrain->left_rail_segment[i]->start) * P1) / (F * P1);
				float g = ((terrain->left_rail_segment[i]->start - motion_segment.start) * P2) / (E * P2);

				if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
				{
					// there is a collision in this time-step
					// if it's less than another collision that occured, set the values
					if (g < collision_time)
					{
						collision_time = g; // it occurs at g of this step
						segment_that_collides = i;
						point_of_intersection = g*motion_segment.from_start + motion_segment.start;
						terrain_that_collides = terrain;
						//index_of_last_track_segment_that_collides = t;
						terrain_segment_where_player_collides = t;
						collides_on_left_terrain = true; // <---!!!important
						collides_through_exit = false;
						collides_through_entrance = false;
					}
				}
			}

			// check the exit

			vec2d &F = track->exit_segment_info->from_start;
			vec2d &P2 = track->exit_segment_info->perpendicular;//Vec2d(-F.y, F.x);

			float h = ((motion_segment.start - track->exit_segment_info->start) * P1) / (F * P1);
			float g = ((track->exit_segment_info->start - motion_segment.start) * P2) / (E * P2);

			if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
			{
				// there is a collision in this time-step
				// if it's less than another collision that occured, set the values
				if (g < collision_time)
				{
					collides_through_exit = true;
					collision_time = g; // it occurs at g of this step
					//segment_that_collides = i;
					point_of_intersection = g*motion_segment.from_start + motion_segment.start;
					terrain_that_collides = NULL;
					collides_through_entrance = false;
					//collides_on_left_terrain = true; // <---!!!important
				}
			}

			// check the entrance to the next segment

			{
				vec2d &F = track->segment[t]->entrance_segment_info->from_start;
				vec2d &P2 = track->segment[t]->entrance_segment_info->perpendicular;//Vec2d(-F.y, F.x);

				float h = ((motion_segment.start - track->segment[t]->entrance_segment_info->start) * P1) / (F * P1);
				float g = ((track->segment[t]->entrance_segment_info->start - motion_segment.start) * P2) / (E * P2);

				if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
				{
					// there is a collision in this time-step
					// if it's less than another collision that occured, set the values
					if (g < collision_time)
					{
						collides_through_entrance = true;
						entrance_that_collides = t;

						collides_through_exit = false;
						collision_time = g; // it occurs at g of this step
						//segment_that_collides = i;
						
						point_of_intersection = g*motion_segment.from_start + motion_segment.start;
						terrain_that_collides = NULL;
						//collides_on_left_terrain = true; // <---!!!important
					}
				}
			}

			// now the right rails

			for (int i=1; i<(int)terrain->right_rail.size(); i++)
			{
				vec2d &F = terrain->right_rail_segment[i]->from_start;//*terrain.point[i] - *terrain.point[i-1];
				vec2d &P2 = terrain->right_rail_segment[i]->perpendicular;//Vec2d(-F.y, F.x);

				float h = ((motion_segment.start - terrain->right_rail_segment[i]->start) * P1) / (F * P1);
				float g = ((terrain->right_rail_segment[i]->start - motion_segment.start) * P2) / (E * P2);

				if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
				{
					// there is a collision in this time-step
					// if it's less than another collision that occured, set the values
					if (g < collision_time)
					{
						collision_time = g; // it occurs at g of this step
						segment_that_collides = i;
						terrain_segment_where_player_collides = t;
						//index_of_last_track_segment_that_collides = t;
						point_of_intersection = g*motion_segment.from_start + motion_segment.start;
						terrain_that_collides = terrain;
						collides_on_left_terrain = false; // <---!!!important
						collides_through_exit = false;
						collides_through_entrance = false;
					}
				}
			}
		}

		//
		// move everything by that timestep
		// adjust velocities of colliding objects
		// decrement timestep by amount within timestep
		//

		player_pos += player_vel*time_left_in_timestep*collision_time;

		if (collides_through_entrance && (__HACK_entrance_already_collided==0))
		{
			//std::cout << "OUT!";
			__HACK_entrance_already_collided++;
			//__HACK_entrance_already_collided %= 10;
			player_pos += player_vel * 0.01; // < this is a solution that causes a *rare* *rare* bug where the player
											 // can escape the track by colliding precicely with the segment entrance and wall
											 // 

			//if (entrance_that_collides < index_of_last_track_segment_that_collides)
			//	index_of_last_track_segment_that_collides--;
			//if (entrance_that_collides > index_of_last_track_segment_that_collides)
				index_of_last_track_segment_that_collides = entrance_that_collides;
				//sound("click").vol(0.5).play();
				al_stop_sample_instance(passthough_sample_instance);
				al_play_sample_instance(passthough_sample_instance);
		}

		if (collides_through_exit)
		{
			index_of_last_track_segment_that_collides = 0;

			//sound("leaving").play();
			//std::cout << std::endl << "warp!!" << std::endl << std::endl;
			player_pos = (track->enter_p2 - track->enter_p1) / 2 + track->enter_p1;
			
			player.complete_lap();

			flash_white();
			
			// rotate velocity and direction
			player.direction_angle += track->exit_segment_info->from_start.GetAngle();
			player_vel = 
				vec2d::PolarCoords(player.velocity.GetAngle() - track->exit_segment_info->from_start.GetAngle(), 1);

			//collision_time*motion_segment.from_start
			//player_vel = 
		}
		else if (terrain_that_collides != NULL) 
		{
				//std::cout << "c";
				// displace along colliding normal (offset padding)
				if (collides_on_left_terrain) player_pos += terrain_that_collides->left_rail_segment[segment_that_collides]->normal * 0.1;
				if (!collides_on_left_terrain) player_pos += terrain_that_collides->right_rail_segment[segment_that_collides]->normal * 0.1;

				// this will bounce the object *while* preserving the x velocity
				//player_vel.x = player_vel.x;
				//player_vel.y = -player_vel.y;

				// this will reflect along the terrain segment (as if a ball bouncing, or light reflecting)
				if (collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->left_rail_segment[segment_that_collides]->normal);
				if (!collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->right_rail_segment[segment_that_collides]->normal);



			switch(terrain_that_collides->color_type)
			{
			case COLOR_TYPE_RED:
				//sound("wall_deflect").vol(1.0).speed(random_float(0.9, 1.1)).play();
				player.velocity_magnitude *= 0.95;
				if (delay_time_since_last_affect < 0)
				{
					play_hit_bad();
					delay_time_since_last_affect = 1.0f;
					player.health -= 20;
				}
				if (player.health < 0) { player.health = 0; kill_player(terrain_segment_where_player_collides); }
				break;

			case COLOR_TYPE_RED_DEATH:
				//sound("wall_deflect").vol(1.0).speed(random_float(0.9, 1.1)).play();
				player.velocity_magnitude *= 0.95;
				player.health = 0;
				play_hit_bad();
				kill_player(terrain_segment_where_player_collides);
				//player.health *= 0.2;
				break;

			case COLOR_TYPE_GREEN:
				//sound("leaving").vol(0.4).play();
				//player.velocity_magnitude *= 0.95;
				if (delay_time_since_last_affect < 0)
				{
					delay_time_since_last_affect = 1.0f;
					player.health += 10.0;
					play_hit_soft();
				}
				if (player.health > player.max_health) player.health = player.max_health;
				break;

			case COLOR_TYPE_BLUE: // bouncy
				//sound("lamp").vol(0.1).speed(random_float(0.9, 1.1)).play();
				//player.velocity_magnitude *= 0.95;
				if (delay_time_since_last_affect < 0)
				{
					play_hit_bounce();
					delay_time_since_last_affect = 1.0f;
					player.velocity_magnitude = 4.5;
				}
				break;

			case COLOR_TYPE_YELLOW:
				//sound("lamp").vol(0.1).speed(random_float(0.9, 1.1)).play();
				player.velocity_magnitude *= 0.95;
				if (delay_time_since_last_affect < 0)
				{
					play_hit_med();
					delay_time_since_last_affect = 1.0f;
					player.health -= 4;
					if (player.health < 0) { player.health = 0; kill_player(terrain_segment_where_player_collides); }
				}
				break;

			//case COLOR_TYPE_OFF:
				//sound("click").play();
				//break;
			default:			
				player.velocity_magnitude *= 0.95;
				//sound("wall_deflect").vol(racer->velocity_magnitude).speed(random_float(0.9, 1.1)).play();
				break;

			// this will respond by adjusing the player's velocity parallel the terrain.
			// The *complete* velocity is used redirected
			//float mag = player_vel.GetMagnitude();
			//vec2d terrain_component = ~-terrain.segment[terrain_that_collides]->from_start;
			//if (player_vel.x > 0) terrain_component *= -1;
			//player_vel = mag * terrain_component;

			// this will respond by adjusing the player's velocity parallel the terrain.
			// The velocity is adjusted to maintain a constant x velocity
			//float mag = player_vel.GetMagnitude();
			//vec2d terrain_component = ~-terrain.segment[terrain_that_collides]->from_start;
			//if (player_vel.x > 0) terrain_component *= -1;
			//float prev_x_vel = player_vel.x;
			//player_vel = mag * terrain_component;
			//player_vel *= (prev_x_vel / player_vel.x);

			// this will respond by adjusing the player's velocity parallel the terrain.
			// The remaining x velocity becomes the length of the velocity along the terrain
			//float mag = player_vel.GetMagnitude();
			//vec2d terrain_component = ~terrain_that_collides->segment[segment_that_collides]->from_start;
			//if (player_vel.x < 0) terrain_component *= -1;
			//float prev_x_vel = player_vel.x;
			//player_vel = mag * terrain_component;
			//player_vel *= (prev_x_vel / player_vel.x);

			//player.on_ground = true;
			//player.image_rotation = terrain_that_collides->segment[segment_that_collides]->normal.GetAngle() + degrees_to_radians(90);
			//player.image_rotation *= 0.5;

			//if(debug) draw_crosshair(player_pos, al_color_name("orange"));
			}
		}

		time_left_in_timestep -= (time_left_in_timestep*collision_time);
		num_steps++;
	}
	
	//if(debug) draw_crosshair(player_pos, al_color_name("red"));

	
	player.position.x = player_pos.x;
	player.position.y = player_pos.y;
	player.velocity.x = player_vel.x;
	player.velocity.y = player_vel.y;

	//if (player.y+player.h > 500)
	//{
		//player.y=500-player.h;
		//player.on_ground = true;
		//player.velocity_y = 0;
	//}
}






/*

void update_racer_and_track(Racer *r, Track *track)
{

	static int every_other = 0;
	every_other++;

	if ((every_other%2) != 0) return;

	static float turn_direction_velocity = 0;
	//if (r->throttle_on) r->velocity_magnitude += 0.065; // good


	if (r->throttle_on) r->velocity_magnitude += 0.085 * OMG_DeltaTime;
	if (r->break_on) r->velocity_magnitude *= 0.9;
	if (r->turning_right) turn_direction_velocity -= 0.01 * OMG_DeltaTime; // good
	if (r->turning_left) turn_direction_velocity += 0.01 * OMG_DeltaTime; // good

	turn_direction_velocity = (turn_direction_velocity * 0.8); // good

	r->direction_angle += turn_direction_velocity * OMG_DeltaTime;

	
	// friction
	//r->velocity_magnitude *= 0.98;
	r->velocity_magnitude *= 0.98; // good


	r->direction = vec2d(sin(r->direction_angle), cos(r->direction_angle));
	r->velocity = (r->velocity*0.5 + r->direction*0.5).Normalized() * r->velocity_magnitude;



	Racer &player = *r;




	static TrackSegment *last_terrain_that_collides = NULL;



	//player.velocity_y += GRAVITY;
	//player.update_walking_velocity();
	//if (!player.moving_left && !player.moving_right) player.velocity_x *= 0.5;
	//player.on_ground = false;

	vec2d start_point = vec2d(player.position.x, player.position.y);
	vec2d end_point = start_point + vec2d(player.velocity.x, player.velocity.y) * player.velocity_magnitude;
	

	vec2d player_pos = start_point;
	vec2d player_vel = end_point - start_point;
	vec2d player_end_pos = end_point;


	//player.position = end_point;
	//return;



	float time_left_in_timestep = 1.0;
	int segment_that_collides = NULL;
	//Terrain *terrain_that_collides = NULL;
	TrackSegment *terrain_that_collides = NULL;
	bool collides_on_left_terrain = false;
	vec2d point_of_intersection;

	int num_steps = 0;

	bool collides_through_exit = false;

	//if(debug) draw_crosshair(player_pos, al_color_name("green"));

	while(time_left_in_timestep > 0)
	{
		//
		// find the soonest intersection time
		//

		float collision_time = 1.0;

		SegmentInfo motion_segment(player_pos, player_pos+player_vel*time_left_in_timestep);

		vec2d &E = motion_segment.from_start;//*terrain.point[i] - *terrain.point[i-1];
		vec2d &P1 = motion_segment.perpendicular;//Vec2d(-F.y, F.x);
		terrain_that_collides = NULL;
		collides_through_exit = false;

		//for (int t=0; t<(int)map.terrain.size(); t++)
		for (int t=0; t<(int)track->segment.size(); t++)
		{
			// just the left rails first

			//Terrain *terrain = map.terrain[t];
			TrackSegment *terrain = track->segment[t];

			
			//for (int i=1; i<(int)terrain->point.size(); i++)
			for (int i=1; i<(int)terrain->left_rail.size(); i++)
			{
				vec2d &F = terrain->left_rail_segment[i]->from_start;//*terrain.point[i] - *terrain.point[i-1];
				vec2d &P2 = terrain->left_rail_segment[i]->perpendicular;//Vec2d(-F.y, F.x);

				float h = ((motion_segment.start - terrain->left_rail_segment[i]->start) * P1) / (F * P1);
				float g = ((terrain->left_rail_segment[i]->start - motion_segment.start) * P2) / (E * P2);

				if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
				{
					// there is a collision in this time-step
					// if it's less than another collision that occured, set the values
					if (g < collision_time)
					{
						collision_time = g; // it occurs at g of this step
						segment_that_collides = i;
						point_of_intersection = g*motion_segment.from_start + motion_segment.start;
						terrain_that_collides = terrain;
						collides_on_left_terrain = true; // <---!!!important
						collides_through_exit = false;
					}
				}
			}

			// check the exit

			vec2d &F = track->exit_segment_info->from_start;
			vec2d &P2 = track->exit_segment_info->perpendicular;//Vec2d(-F.y, F.x);

			float h = ((motion_segment.start - track->exit_segment_info->start) * P1) / (F * P1);
			float g = ((track->exit_segment_info->start - motion_segment.start) * P2) / (E * P2);

			if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
			{
				// there is a collision in this time-step
				// if it's less than another collision that occured, set the values
				if (g < collision_time)
				{
					collides_through_exit = true;
					collision_time = g; // it occurs at g of this step
					//segment_that_collides = i;
					point_of_intersection = g*motion_segment.from_start + motion_segment.start;
					terrain_that_collides = NULL;
					//collides_on_left_terrain = true; // <---!!!important
				}
			}

			// now the right rails

			for (int i=1; i<(int)terrain->right_rail.size(); i++)
			{
				vec2d &F = terrain->right_rail_segment[i]->from_start;//*terrain.point[i] - *terrain.point[i-1];
				vec2d &P2 = terrain->right_rail_segment[i]->perpendicular;//Vec2d(-F.y, F.x);

				float h = ((motion_segment.start - terrain->right_rail_segment[i]->start) * P1) / (F * P1);
				float g = ((terrain->right_rail_segment[i]->start - motion_segment.start) * P2) / (E * P2);

				if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
				{
					// there is a collision in this time-step
					// if it's less than another collision that occured, set the values
					if (g < collision_time)
					{
						collision_time = g; // it occurs at g of this step
						segment_that_collides = i;
						point_of_intersection = g*motion_segment.from_start + motion_segment.start;
						terrain_that_collides = terrain;
						collides_on_left_terrain = false; // <---!!!important
						collides_through_exit = false;
					}
				}
			}
		}

		//
		// move everything by that timestep
		// adjust velocities of colliding objects
		// decrement timestep by amount within timestep
		//

		player_pos += player_vel*time_left_in_timestep*collision_time;

		if (collides_through_exit)
		{
			//sound("leaving").play();
			//std::cout << std::endl << "warp!!" << std::endl << std::endl;
			player_pos = (track->enter_p2 - track->enter_p1) / 2 + track->enter_p1;
			
			player.complete_lap();
			
			// rotate velocity and direction
			player.direction_angle += track->exit_segment_info->from_start.GetAngle();
			player_vel = 
				vec2d::PolarCoords(player.velocity.GetAngle() - track->exit_segment_info->from_start.GetAngle(), 1);

			//collision_time*motion_segment.from_start
			//player_vel = 
		}
		else if (terrain_that_collides != NULL) 
		{
				//std::cout << "c";
				// displace along colliding normal (offset padding)
				if (collides_on_left_terrain) player_pos += terrain_that_collides->left_rail_segment[segment_that_collides]->normal * 0.1;
				if (!collides_on_left_terrain) player_pos += terrain_that_collides->right_rail_segment[segment_that_collides]->normal * 0.1;

				// this will bounce the object *while* preserving the x velocity
				//player_vel.x = player_vel.x;
				//player_vel.y = -player_vel.y;

				// this will reflect along the terrain segment (as if a ball bouncing, or light reflecting)
				if (collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->left_rail_segment[segment_that_collides]->normal);
				if (!collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->right_rail_segment[segment_that_collides]->normal);

				player.velocity_magnitude *= 0.95;


			switch(terrain_that_collides->color_type)
			{
			case COLOR_TYPE_RED:
				//sound("wall_deflect").vol(1.0).play();
				player.health -= 10;
				if (player.health < 0) kill_player();
				break;

			case COLOR_TYPE_RED_DEATH:
				//sound("wall_deflect").vol(1.0).play();
				player.health *= 0.2;
				break;

			case COLOR_TYPE_GREEN:
				//sound("lamp").vol(0.1).speed(random_float(0.9, 1.1)).play();
				player.health += 2.0;
				if (player.health > player.max_health) player.health = player.max_health;
				break;

			case COLOR_TYPE_BLUE: // bouncy
				//sound("lamp").vol(0.1).speed(random_float(0.9, 1.1)).play();
				//player.velocity_magnitude += 1.0;
				break;

			case COLOR_TYPE_YELLOW: // bouncy
				//sound("lamp").vol(0.1).speed(random_float(0.9, 1.1)).play();
				player.health -= 5;
				if (player.health < 0) kill_player();
				//player.velocity_magnitude += 1.0;
				break;

			default:			
				//sound("wall_deflect").vol(racer->velocity_magnitude).speed(random_float(0.9, 1.1)).play();
				break;

			// this will respond by adjusing the player's velocity parallel the terrain.
			// The *complete* velocity is used redirected
			//float mag = player_vel.GetMagnitude();
			//vec2d terrain_component = ~-terrain.segment[terrain_that_collides]->from_start;
			//if (player_vel.x > 0) terrain_component *= -1;
			//player_vel = mag * terrain_component;

			// this will respond by adjusing the player's velocity parallel the terrain.
			// The velocity is adjusted to maintain a constant x velocity
			//float mag = player_vel.GetMagnitude();
			//vec2d terrain_component = ~-terrain.segment[terrain_that_collides]->from_start;
			//if (player_vel.x > 0) terrain_component *= -1;
			//float prev_x_vel = player_vel.x;
			//player_vel = mag * terrain_component;
			//player_vel *= (prev_x_vel / player_vel.x);

			// this will respond by adjusing the player's velocity parallel the terrain.
			// The remaining x velocity becomes the length of the velocity along the terrain
			//float mag = player_vel.GetMagnitude();
			//vec2d terrain_component = ~terrain_that_collides->segment[segment_that_collides]->from_start;
			//if (player_vel.x < 0) terrain_component *= -1;
			//float prev_x_vel = player_vel.x;
			//player_vel = mag * terrain_component;
			//player_vel *= (prev_x_vel / player_vel.x);

			//player.on_ground = true;
			//player.image_rotation = terrain_that_collides->segment[segment_that_collides]->normal.GetAngle() + degrees_to_radians(90);
			//player.image_rotation *= 0.5;

			//if(debug) draw_crosshair(player_pos, al_color_name("orange"));
			}
		}

		time_left_in_timestep -= (time_left_in_timestep*collision_time);
		num_steps++;
	}
	
	//if(debug) draw_crosshair(player_pos, al_color_name("red"));

	
	player.position.x = player_pos.x;
	player.position.y = player_pos.y;
	player.velocity.x = player_vel.x;
	player.velocity.y = player_vel.y;

	//if (player.y+player.h > 500)
	//{
		//player.y=500-player.h;
		//player.on_ground = true;
		//player.velocity_y = 0;
	//}
}

*/















/*


void ORIGINAL_update_racer_and_track(Racer *r, Track *track)
{
	//static bool every_other = false;
	//every_other = !every_other;

	static float turn_direction_velocity = 0;
	//if (r->throttle_on) r->velocity_magnitude += 0.065; // good


	if (r->throttle_on) r->velocity_magnitude += 0.085;
	if (r->break_on) r->velocity_magnitude *= 0.9;
	if (r->turning_right) turn_direction_velocity -= 0.01; // good
	if (r->turning_left) turn_direction_velocity += 0.01; // good

	turn_direction_velocity *= 0.8; // good

	r->direction_angle += turn_direction_velocity;

	r->direction = vec2d(sin(r->direction_angle), cos(r->direction_angle));
	
	// friction
	//r->velocity_magnitude *= 0.98;
	r->velocity_magnitude *= 0.98 * OMG_DeltaTime; // good


	r->velocity = (r->velocity*0.5 + r->direction*0.5).Normalized() * r->velocity_magnitude;



	Racer &player = *r;





	//player.velocity_y += GRAVITY;
	//player.update_walking_velocity();
	//if (!player.moving_left && !player.moving_right) player.velocity_x *= 0.5;
	//player.on_ground = false;

	vec2d start_point = vec2d(player.position.x, player.position.y);
	vec2d end_point = start_point + vec2d(player.velocity.x, player.velocity.y) * player.velocity_magnitude;
	

	vec2d player_pos = start_point;
	vec2d player_vel = end_point - start_point;
	vec2d player_end_pos = end_point;


	//player.position = end_point;
	//return;



	float time_left_in_timestep = 1.0;
	int segment_that_collides = NULL;
	//Terrain *terrain_that_collides = NULL;
	TrackSegment *terrain_that_collides = NULL;
	bool collides_on_left_terrain = false;
	vec2d point_of_intersection;

	int num_steps = 0;

	bool collides_through_exit = false;

	//if(debug) draw_crosshair(player_pos, al_color_name("green"));

	while(time_left_in_timestep > 0)
	{
		//
		// find the soonest intersection time
		//

		float collision_time = 1.0;

		SegmentInfo motion_segment(player_pos, player_pos+player_vel*time_left_in_timestep);

		vec2d &E = motion_segment.from_start;//*terrain.point[i] - *terrain.point[i-1];
		vec2d &P1 = motion_segment.perpendicular;//Vec2d(-F.y, F.x);
		terrain_that_collides = NULL;
		collides_through_exit = false;

		//for (int t=0; t<(int)map.terrain.size(); t++)
		for (int t=0; t<(int)track->segment.size(); t++)
		{
			// just the left rails first

			//Terrain *terrain = map.terrain[t];
			TrackSegment *terrain = track->segment[t];

			
			//for (int i=1; i<(int)terrain->point.size(); i++)
			for (int i=1; i<(int)terrain->left_rail.size(); i++)
			{
				vec2d &F = terrain->left_rail_segment[i]->from_start;//*terrain.point[i] - *terrain.point[i-1];
				vec2d &P2 = terrain->left_rail_segment[i]->perpendicular;//Vec2d(-F.y, F.x);

				float h = ((motion_segment.start - terrain->left_rail_segment[i]->start) * P1) / (F * P1);
				float g = ((terrain->left_rail_segment[i]->start - motion_segment.start) * P2) / (E * P2);

				if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
				{
					// there is a collision in this time-step
					// if it's less than another collision that occured, set the values
					if (g < collision_time)
					{
						collision_time = g; // it occurs at g of this step
						segment_that_collides = i;
						point_of_intersection = g*motion_segment.from_start + motion_segment.start;
						terrain_that_collides = terrain;
						collides_on_left_terrain = true; // <---!!!important
						collides_through_exit = false;
					}
				}
			}

			// check the exit

			vec2d &F = track->exit_segment_info->from_start;
			vec2d &P2 = track->exit_segment_info->perpendicular;//Vec2d(-F.y, F.x);

			float h = ((motion_segment.start - track->exit_segment_info->start) * P1) / (F * P1);
			float g = ((track->exit_segment_info->start - motion_segment.start) * P2) / (E * P2);

			if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
			{
				// there is a collision in this time-step
				// if it's less than another collision that occured, set the values
				if (g < collision_time)
				{
					collides_through_exit = true;
					collision_time = g; // it occurs at g of this step
					//segment_that_collides = i;
					point_of_intersection = g*motion_segment.from_start + motion_segment.start;
					terrain_that_collides = NULL;
					//collides_on_left_terrain = true; // <---!!!important
				}
			}

			// now the right rails

			for (int i=1; i<(int)terrain->right_rail.size(); i++)
			{
				vec2d &F = terrain->right_rail_segment[i]->from_start;//*terrain.point[i] - *terrain.point[i-1];
				vec2d &P2 = terrain->right_rail_segment[i]->perpendicular;//Vec2d(-F.y, F.x);

				float h = ((motion_segment.start - terrain->right_rail_segment[i]->start) * P1) / (F * P1);
				float g = ((terrain->right_rail_segment[i]->start - motion_segment.start) * P2) / (E * P2);

				if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
				{
					// there is a collision in this time-step
					// if it's less than another collision that occured, set the values
					if (g < collision_time)
					{
						collision_time = g; // it occurs at g of this step
						segment_that_collides = i;
						point_of_intersection = g*motion_segment.from_start + motion_segment.start;
						terrain_that_collides = terrain;
						collides_on_left_terrain = false; // <---!!!important
						collides_through_exit = false;
					}
				}
			}
		}

		//
		// move everything by that timestep
		// adjust velocities of colliding objects
		// decrement timestep by amount within timestep
		//

		player_pos += player_vel*time_left_in_timestep*collision_time;

		if (collides_through_exit)
		{
			sound("leaving").play();
			//std::cout << std::endl << "warp!!" << std::endl << std::endl;
			player_pos = (track->enter_p2 - track->enter_p1) / 2 + track->enter_p1;
			
			player.complete_lap();
			
			// rotate velocity and direction
			player.direction_angle += track->exit_segment_info->from_start.GetAngle();
			player_vel = 
				vec2d::PolarCoords(player.velocity.GetAngle() - track->exit_segment_info->from_start.GetAngle(), 1);

			//collision_time*motion_segment.from_start
			//player_vel = 
		}
		else if (terrain_that_collides != NULL) 
		{
			switch(terrain_that_collides->color_type)
			{
			case COLOR_TYPE_RED_DEATH:
				sound("wall_deflect").vol(1.0).play();
					//std::cout << "c";
					// displace along colliding normal (offset padding)
					if (collides_on_left_terrain) player_pos += terrain_that_collides->left_rail_segment[segment_that_collides]->normal * 0.1;
					if (!collides_on_left_terrain) player_pos += terrain_that_collides->right_rail_segment[segment_that_collides]->normal * 0.1;

					// this will bounce the object *while* preserving the x velocity
					//player_vel.x = player_vel.x;
					//player_vel.y = -player_vel.y;

					// this will reflect along the terrain segment (as if a ball bouncing, or light reflecting)
					if (collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->left_rail_segment[segment_that_collides]->normal);
					if (!collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->right_rail_segment[segment_that_collides]->normal);

					//player_vel = player_vel.Normalized() * 100;
					player.velocity_magnitude *= 0.3;
				break;

			case COLOR_TYPE_GREEN:
				{
					sound("lamp").vol(0.1).speed(random_float(0.9, 1.1)).play();

					//std::cout << "c";
					// displace along colliding normal (offset padding)
					if (collides_on_left_terrain) player_pos += terrain_that_collides->left_rail_segment[segment_that_collides]->normal * 0.1;
					if (!collides_on_left_terrain) player_pos += terrain_that_collides->right_rail_segment[segment_that_collides]->normal * 0.1;

					// this will bounce the object *while* preserving the x velocity
					//player_vel.x = player_vel.x;
					//player_vel.y = -player_vel.y;

					// this will reflect along the terrain segment (as if a ball bouncing, or light reflecting)
					if (collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->left_rail_segment[segment_that_collides]->normal);
					if (!collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->right_rail_segment[segment_that_collides]->normal);

					player.velocity_magnitude *= 0.95;
				}
				break;

			case COLOR_TYPE_BLUE: // bouncy
				{
					sound("lamp").vol(0.1).speed(random_float(0.9, 1.1)).play();

					//std::cout << "c";
					// displace along colliding normal (offset padding)
					if (collides_on_left_terrain) player_pos += terrain_that_collides->left_rail_segment[segment_that_collides]->normal * 0.1;
					if (!collides_on_left_terrain) player_pos += terrain_that_collides->right_rail_segment[segment_that_collides]->normal * 0.1;

					// this will bounce the object *while* preserving the x velocity
					//player_vel.x = player_vel.x;
					//player_vel.y = -player_vel.y;

					// this will reflect along the terrain segment (as if a ball bouncing, or light reflecting)
					if (collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->left_rail_segment[segment_that_collides]->normal);
					if (!collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->right_rail_segment[segment_that_collides]->normal);

					player.velocity_magnitude *= 1.1;
				}
				break;

			default:			
				sound("wall_deflect").vol(racer->velocity_magnitude).speed(random_float(0.9, 1.1)).play();
				create_particle_spread(racer->position.x, racer->position.y);

				//std::cout << "c";
				// displace along colliding normal (offset padding)
				if (collides_on_left_terrain) player_pos += terrain_that_collides->left_rail_segment[segment_that_collides]->normal * 0.1;
				if (!collides_on_left_terrain) player_pos += terrain_that_collides->right_rail_segment[segment_that_collides]->normal * 0.1;

				// this will bounce the object *while* preserving the x velocity
				//player_vel.x = player_vel.x;
				//player_vel.y = -player_vel.y;

				// this will reflect along the terrain segment (as if a ball bouncing, or light reflecting)
				if (collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->left_rail_segment[segment_that_collides]->normal);
				if (!collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->right_rail_segment[segment_that_collides]->normal);

				player.velocity_magnitude *= 0.7;
				break;

			// this will respond by adjusing the player's velocity parallel the terrain.
			// The *complete* velocity is used redirected
			//float mag = player_vel.GetMagnitude();
			//vec2d terrain_component = ~-terrain.segment[terrain_that_collides]->from_start;
			//if (player_vel.x > 0) terrain_component *= -1;
			//player_vel = mag * terrain_component;

			// this will respond by adjusing the player's velocity parallel the terrain.
			// The velocity is adjusted to maintain a constant x velocity
			//float mag = player_vel.GetMagnitude();
			//vec2d terrain_component = ~-terrain.segment[terrain_that_collides]->from_start;
			//if (player_vel.x > 0) terrain_component *= -1;
			//float prev_x_vel = player_vel.x;
			//player_vel = mag * terrain_component;
			//player_vel *= (prev_x_vel / player_vel.x);

			// this will respond by adjusing the player's velocity parallel the terrain.
			// The remaining x velocity becomes the length of the velocity along the terrain
			//float mag = player_vel.GetMagnitude();
			//vec2d terrain_component = ~terrain_that_collides->segment[segment_that_collides]->from_start;
			//if (player_vel.x < 0) terrain_component *= -1;
			//float prev_x_vel = player_vel.x;
			//player_vel = mag * terrain_component;
			//player_vel *= (prev_x_vel / player_vel.x);

			//player.on_ground = true;
			//player.image_rotation = terrain_that_collides->segment[segment_that_collides]->normal.GetAngle() + degrees_to_radians(90);
			//player.image_rotation *= 0.5;

			//if(debug) draw_crosshair(player_pos, al_color_name("orange"));
			}
		}

		time_left_in_timestep -= (time_left_in_timestep*collision_time);
		num_steps++;
	}
	
	//if(debug) draw_crosshair(player_pos, al_color_name("red"));

	
	player.position.x = player_pos.x;
	player.position.y = player_pos.y;
	player.velocity.x = player_vel.x;
	player.velocity.y = player_vel.y;

	//if (player.y+player.h > 500)
	//{
		//player.y=500-player.h;
		//player.on_ground = true;
		//player.velocity_y = 0;
	//}
}

*/