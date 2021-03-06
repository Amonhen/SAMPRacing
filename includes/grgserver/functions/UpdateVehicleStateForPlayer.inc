UpdateVehicleStateForPlayer(playerID)
{
	new string[256];

	new vehicleID = GetPlayerVehicleID(playerID);

	// Speed calculcation
	new Float:velocityX;
	new Float:velocityY;
	new Float:velocityZ;
	GetVehicleVelocity(vehicleID, velocityX, velocityY, velocityZ);
	new speed = floatround(floatsqroot(floatpower(velocityX, 2) + floatpower(velocityY, 2) + floatpower(velocityZ, 2)) * 100 * 1.61);

	// Player is in a vehicle
	if (vehicleID)
	{
		new Float:health;
		GetVehicleHealth(vehicleID, health);

		// Fix infinite health
		if (health > 1000)
		{
			health = 1000;
		}

		// 250 or less = Vehicle is burning
		if (health < 250)
		{
			health = 250;
		}

		new backgroundColor;
		new color;
		if (health > 750)
		{
			backgroundColor = 0x006600ff;
			color = 0x00aa00ff;
		}
		else if (health > 500)
		{
			backgroundColor = 0x666600ff;
			color = 0xaaaa00ff;
		}
		else
		{
			backgroundColor = 0x660000ff;
			color = 0xaa0000ff;
		}

		PlayerTextDrawTextSize(playerID, g_playerTextDraws[playerID][E_PTD_VEHICLE_HEALTH_VALUE], floatadd(SPEEDOMETER_HEALTHBAR_POSX, floatmul(floatsub(health, 250), floatdiv(SPEEDOMETER_HEALTHBAR_LENGTH, 750))), 0.0);
		PlayerTextDrawBoxColor(playerID, g_playerTextDraws[playerID][E_PTD_VEHICLE_HEALTH_VALUE], color);
		PlayerTextDrawBoxColor(playerID, g_playerTextDraws[playerID][E_PTD_VEHICLE_HEALTH_BOX2], backgroundColor);

		// Show vehicle health
		PlayerTextDrawShow(playerID, g_playerTextDraws[playerID][E_PTD_VEHICLE_HEALTH_BOX1]);
		PlayerTextDrawShow(playerID, g_playerTextDraws[playerID][E_PTD_VEHICLE_HEALTH_BOX2]);
		PlayerTextDrawShow(playerID, g_playerTextDraws[playerID][E_PTD_VEHICLE_HEALTH_VALUE]);

		// Update and show analog speedometer
		Count(step, MAX_SPEEDOMETER_STEPS)
		{
			new stepSpeed = 20 * (step + 1);
			Format:string("~%c~%d", speed >= stepSpeed ? 'r' : 'g', stepSpeed);
			PlayerTextDrawSetString(playerID, g_playerTextDraws[playerID][E_PTD_SPEEDOMETER][step], string);
			PlayerTextDrawShow(playerID, g_playerTextDraws[playerID][E_PTD_SPEEDOMETER][step]);
		}

		// Update and show digital speedometer
		Format:string("%d", speed);
		PlayerTextDrawSetString(playerID, g_playerTextDraws[playerID][E_PTD_SPEEDOMETER_VALUE], string);
		PlayerTextDrawShow(playerID, g_playerTextDraws[playerID][E_PTD_SPEEDOMETER_VALUE]);
		PlayerTextDrawShow(playerID, g_playerTextDraws[playerID][E_PTD_SPEEDOMETER_UNIT]);
	}
	else
	{
		// Hide vehicle health
		PlayerTextDrawHide(playerID, g_playerTextDraws[playerID][E_PTD_VEHICLE_HEALTH_BOX1]);
		PlayerTextDrawHide(playerID, g_playerTextDraws[playerID][E_PTD_VEHICLE_HEALTH_BOX2]);
		PlayerTextDrawHide(playerID, g_playerTextDraws[playerID][E_PTD_VEHICLE_HEALTH_VALUE]);

		// Hide speedometer
		Count(step, MAX_SPEEDOMETER_STEPS)
		{
			PlayerTextDrawHide(playerID, g_playerTextDraws[playerID][E_PTD_SPEEDOMETER][step]);
		}
		PlayerTextDrawHide(playerID, g_playerTextDraws[playerID][E_PTD_SPEEDOMETER_VALUE]);
		PlayerTextDrawHide(playerID, g_playerTextDraws[playerID][E_PTD_SPEEDOMETER_UNIT]);
	}

	// Drifting is enabled and player is the driver
	if (PVar:playerID[DRIFTING_STATE] && GetPlayerVehicleSeat(playerID) == 0)
	{
		if (PVar:playerID[DRIFTING_RESETSTATSTIMER])
		{
			KillTimer(PVar:playerID[DRIFTING_RESETSTATSTIMER]);
			PVar:playerID[DRIFTING_RESETSTATSTIMER] = 0;
		}

		if (speed >= 30)
		{
			// Get direction
			new Float:direction;
			if (velocityX < 0)
			{
				if (velocityY > 0)
				{
					direction = atan(floatabs(velocityX / velocityY));
				}
				else if (velocityY <= 0)
				{
					direction = atan(velocityY / velocityX) + 90;
				}
			}
			else if (velocityX > 0)
			{
				if (velocityY < 0)
				{
					direction = atan(floatabs(velocityX / velocityY)) + 180;
				}
				else if (velocityY >= 0)
				{
					direction = atan(velocityY / velocityX) + 270;
				}
			}
			else if (velocityX == 0)
			{
				if (velocityY > 0)
				{ 
					direction = 0;
				}
				else if (velocityY < 0)
				{ 
					direction = 180;
				}
			}

			// Calculate drifting angle
			new Float:currentDirection;
			GetVehicleZAngle(vehicleID, currentDirection);
			new Float:angle = floatabs(direction - currentDirection);

			// Calculate score
			if ((15 < angle < 80) || ( 280 < angle < 345))
			{
				PVar:playerID[DRIFTING_SCORESTATE] = true;
				PVar:playerID[DRIFTING_SCORE]++;
				GivePlayerMoney(playerID, PVar:playerID[DRIFTING_SCORE] / 10);

				if ((25 < angle < 70) || (290 < angle < 335))
				{
					PVar:playerID[DRIFTING_SCORE]++;
					if ((35 < angle < 45) || (315 < angle < 325))
					{
						PVar:playerID[DRIFTING_SCORE]++;
						PVar:playerID[DRIFTING_ANGLEBONUS]++;
					}
				}

				if (speed > 100)
				{
					PVar:playerID[DRIFTING_SCORE]++;
					PVar:playerID[DRIFTING_SPEEDBONUS]++;
				}
			}
			// Player stops drifting
			else if (PVar:playerID[DRIFTING_SCORE])
			{
				if (PVar:playerID[DRIFTING_SCORESTATE])
				{
					if (PVar:playerID[DRIFTING_SCORE] > PVar:playerID[DRIFTING_MAXSCORE])
					{
						PVar:playerID[DRIFTING_MAXSCORE] = PVar:playerID[DRIFTING_SCORE];
						if (PVar:playerID[DRIFTING_SCORE] > PVar:playerID[DRIFTING_BESTSCORE])
						{
							PVar:playerID[DRIFTING_BESTSCORE] = PVar:playerID[DRIFTING_SCORE];
						}
					}
					else
					{
						if (PVar:playerID[DRIFTING_SCORE] > 70)
						{
							GameTextForPlayer(playerID, LanguageString(playerID, StringID:150("Nice drifting!")), 3000, 6);
							RepairVehicle(vehicleID);
							SetVehicleHealth(vehicleID, 1000);
							AddVehicleComponent(vehicleID, 1009);
						}
					}
					PVar:playerID[DRIFTING_LASTSCORE] = PVar:playerID[DRIFTING_SCORE];
					PVar:playerID[DRIFTING_SCORESTATE] = false;

					GivePlayerMoney(playerID, PVar:playerID[DRIFTING_ANGLEBONUS] / 10);
					GivePlayerMoney(playerID, PVar:playerID[DRIFTING_SPEEDBONUS] / 5);
					GivePlayerMoney(playerID, PVar:playerID[DRIFTING_SCORE] / 2);
				}

				// Reset values
				PVar:playerID[DRIFTING_SPEEDBONUS] = 0;
				PVar:playerID[DRIFTING_ANGLEBONUS] = 0;

				PVar:playerID[DRIFTING_SCORE]--;
			}
		}
		else if (PVar:playerID[DRIFTING_SCORE])
		{
			if (PVar:playerID[DRIFTING_SCORE] > PVar:playerID[DRIFTING_BESTSCORE])
			{
				PVar:playerID[DRIFTING_BESTSCORE] = PVar:playerID[DRIFTING_SCORE];
			}

			PVar:playerID[DRIFTING_LASTSCORE] = PVar:playerID[DRIFTING_SCORE];

			// Reset values
			PVar:playerID[DRIFTING_SCORE] = 0;
			PVar:playerID[DRIFTING_SPEEDBONUS] = 0;
			PVar:playerID[DRIFTING_ANGLEBONUS] = 0;
		}

		Format:string(LanguageString(playerID, StringID:43("Drift Score: %d~n~~n~Best Score: %d * Last Score: %d")), PVar:playerID[DRIFTING_SCORE], PVar:playerID[DRIFTING_BESTSCORE], PVar:playerID[DRIFTING_LASTSCORE]);
		PlayerTextDrawSetString(playerID, g_playerTextDraws[playerID][E_PTD_DRIFTING_BOTTOM], string);

		Format:string("%d~n~~n~%d", PVar:playerID[DRIFTING_SPEEDBONUS], PVar:playerID[DRIFTING_ANGLEBONUS]);
		PlayerTextDrawSetString(playerID, g_playerTextDraws[playerID][E_PTD_DRIFTING_BONUS_VALUES], string);

		PlayerTextDrawShow(playerID, g_playerTextDraws[playerID][E_PTD_DRIFTING_BONUS_LABELS]);
		PlayerTextDrawShow(playerID, g_playerTextDraws[playerID][E_PTD_DRIFTING_BONUS_VALUES]);
		PlayerTextDrawShow(playerID, g_playerTextDraws[playerID][E_PTD_DRIFTING_BOTTOM]);

		PVar:playerID[DRIFTING_WASDRIFTING] = true;
	}
	else
	{
		if (PVar:playerID[DRIFTING_WASDRIFTING])
		{
			PVar:playerID[DRIFTING_WASDRIFTING] = false;

			PVar:playerID[DRIFTING_RESETSTATSTIMER] = SetTimerEx("Timer_ResetDriftingStats", TIMER_RESETDRIFTINGSTATS, false, "d", playerID);

			PlayerTextDrawHide(playerID, g_playerTextDraws[playerID][E_PTD_DRIFTING_BONUS_LABELS]);
			PlayerTextDrawHide(playerID, g_playerTextDraws[playerID][E_PTD_DRIFTING_BONUS_VALUES]);
			PlayerTextDrawHide(playerID, g_playerTextDraws[playerID][E_PTD_DRIFTING_BOTTOM]);
		}
	}
}