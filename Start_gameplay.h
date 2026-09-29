

void start_gameplay()
{

    switch (level)
    {
    case ZERO_WINDOW:

        mousepos = GetMousePosition();
        {
            Color playBtnColor = GetButtonColor(play_button_posRec, mousepos, Button_color);
            Color credentialBtnColor = GetButtonColor(credentialButton_posRec, mousepos, Button_color);
            Color leaderboardBtnColor = GetButtonColor(leaderboard_button_posRec, mousepos, Button_color);
            Color exitBtnColor = GetButtonColor(main_exit_button_posRec, mousepos, Button_color);
            Color howtoBtnColor = GetButtonColor(howto_button_posRec, mousepos, Button_color);

            DrawTexturePro(space_background,
                           (Rectangle){0, 0, space_background.width, space_background.height},
                           (Rectangle){0, 0, screen_width, screen_height},
                           (Vector2){0, 0}, 0.0f, WHITE);

            DrawRectangleRounded(play_button_posRec, 1.0f, 8, playBtnColor);
            DrawRectangleRoundedLinesEx(play_button_posRec, 1.0f, 8, 2, BLACK);
            DrawRectangleRounded(game_title_posRec, 1.0f, 8, Button_color);
            DrawRectangleRoundedLinesEx(game_title_posRec, 1.0f, 8, 4, BLACK);
            DrawRectangleRounded(credentialButton_posRec, 1.0f, 8, credentialBtnColor);
            DrawRectangleRoundedLinesEx(credentialButton_posRec, 1.0f, 8, 2, BLACK);
            DrawRectangleRounded(leaderboard_button_posRec, 1.0f, 8, leaderboardBtnColor);
            DrawRectangleRoundedLinesEx(leaderboard_button_posRec, 1.0f, 8, 2, BLACK);
            DrawRectangleRounded(howto_button_posRec, 1.0f, 8, howtoBtnColor);
            DrawRectangleRoundedLinesEx(howto_button_posRec, 1.0f, 8, 2, BLACK);
            DrawRectangleRounded(main_exit_button_posRec, 1.0f, 8, exitBtnColor);
            DrawRectangleRoundedLinesEx(main_exit_button_posRec, 1.0f, 8, 2, BLACK);
        }

        DrawTextEx(font_play, play_message, play_button_pos, (float)font_play.baseSize + 10, 2, BLUE);
        DrawTextEx(font_play, game_title, game_title_pos, (float)font_play.baseSize + 40, 4, BLUE);
        DrawTextEx(font_play, credential_title, credentialButton_pos, (float)font_play.baseSize + 10, 2, BLUE);
        DrawTextEx(font_play, leaderboard_button_message, leaderboard_button_pos, (float)font_play.baseSize + 10, 2, BLUE);
        DrawTextEx(font_play, howto_button_message, howto_button_pos, (float)font_play.baseSize + 10, 2, BLUE);
        DrawTextEx(font_play, exit_message, main_exit_button_pos, (float)font_play.baseSize + 10, 2, BLUE);

        if (CheckCollisionPointRec(mousepos, play_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            currentLevelNumber = 1;
            scoreSaved = false; // a fresh run hasn't had its average score saved yet
            StartLevelTimer();
            level = NAME_INPUT;
        }

        if (CheckCollisionPointRec(mousepos, credentialButton_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = CREDENTIAL;
        }

        if (CheckCollisionPointRec(mousepos, leaderboard_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            LoadPlayerLeaderboard();
            level = LEADERBOARD_WINDOW;
        }
        if (CheckCollisionPointRec(mousepos, howto_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = HOW_TO_PLAY_WINDOW;
        }

        if (CheckCollisionPointRec(mousepos, main_exit_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            exitGameRequested = true;
        }

        break;

    case LEADERBOARD_WINDOW:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTextEx(font_play, leaderboard_title, leaderboard_title_pos, (float)font_play.baseSize + 20, 2, GOLD);

        if (playerScoreCount == 0)
        {
            char *noScoresMsg = "No completed runs yet - finish all 4 levels to set a score!";
            Vector2 noScoresSize = MeasureTextEx(font_play, noScoresMsg, 24, spacing);
            DrawTextEx(font_play, noScoresMsg,
                       (Vector2){screen_width / 2 - noScoresSize.x / 2, screen_height / 2 - noScoresSize.y / 2},
                       24, spacing, GOLD);
        }
        else
        {
            float rowGap = 40;
            float rowsStartY = leaderboard_title_pos.y + 90;
            int rowsToShow = (playerScoreCount < LEADERBOARD_DISPLAY_COUNT) ? playerScoreCount : LEADERBOARD_DISPLAY_COUNT;

            for (int i = 0; i < rowsToShow; i++)
            {
                char rowText[64];
                snprintf(rowText, sizeof(rowText), "%2d.  %-16s  %s",
                         i + 1, playerScores[i].name, FormatTime(playerScores[i].avgTime));

                Vector2 rowSize = MeasureTextEx(font_play, rowText, 26, spacing);
                Color rowColor = (i == 0) ? GOLD : (Color){170, 210, 255, 255}; // highlight the #1 spot

                DrawTextEx(font_play, rowText,
                           (Vector2){screen_width / 2 - rowSize.x / 2, rowsStartY + i * rowGap},
                           26, spacing, rowColor);
            }
        }

        {
            Color leaderboardBackBtnColor = GetButtonColor(leaderboard_back_posRec, mousepos, Button_color);
            DrawRectangleRounded(leaderboard_back_posRec, 1.0f, 8, leaderboardBackBtnColor);
        }
        DrawTextEx(font_play, leaderboard_back_message, leaderboard_back_pos, (float)font_play.baseSize + 30, 2, BLUE);

        if (CheckCollisionPointRec(mousepos, leaderboard_back_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = ZERO_WINDOW;
        }

        break;

    case HOW_TO_PLAY_WINDOW:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTextEx(font_play, howto_title, howto_title_pos, (float)font_play.baseSize + 20, 2, GOLD);

        DrawRectangleRounded(howto_box, 0.05f, 8, (Color){10, 15, 40, 230});
        DrawRectangleRoundedLinesEx(howto_box, 0.05f, 8, 2, GOLD);

        for (int i = 0; i < HOWTO_LINE_COUNT; i++)
        {
            DrawTextEx(font_play, howto_lines[i], howto_line_pos[i], 26, spacing, GOLD);
        }

        {
            Color howtoBackColor = GetButtonColor(howto_back_posRec, mousepos, Button_color);
            DrawRectangleRounded(howto_back_posRec, 1.0f, 8, howtoBackColor);
        }
        DrawTextEx(font_play, back_message, howto_back_pos, (float)font_play.baseSize + 30, 2, BLUE);

        if (CheckCollisionPointRec(mousepos, howto_back_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = ZERO_WINDOW;
        }

        break;

    case CREDENTIAL:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTextEx(font_play, credential_name1, credential_name1_pos, (float)font_play.baseSize + 10, 2, GOLD);
        DrawTextEx(font_play, credential_name2, credential_name2_pos, (float)font_play.baseSize + 10, 2, GOLD);
        DrawTextEx(font_play, supervisor_label, supervisor_label_pos, (float)font_play.baseSize + 10, 2, GOLD);
        DrawTextEx(font_play, supervisor_name, supervisor_name_pos, (float)font_play.baseSize + 10, 2, GOLD);

        {
            Color backBtnColor = GetButtonColor(credential_back_posRec, mousepos, Button_color);
            DrawRectangleRounded(credential_back_posRec, 1.0f, 8, backBtnColor);
        }
        DrawTextEx(font_play, back_message, credential_back_pos, (float)font_play.baseSize + 30, 2, BLUE);

        if (CheckCollisionPointRec(mousepos, credential_back_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = ZERO_WINDOW;
        }
        break;

    case NAME_INPUT:
    {
        mousepos = GetMousePosition();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawRectangleRounded(name_label_box, 0.3f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(name_label_box, 0.3f, 8, 2, BLACK);
        DrawTextEx(font_play, name_label, name_label_pos, font_size, spacing, GOLD);

        DrawRectangleRounded(name_input_box, 0.3f, 8, (Color){10, 15, 40, 255});
        DrawRectangleRoundedLinesEx(name_input_box, 0.3f, 8, 2, BLACK);
        DrawTextEx(font_play, playerName, name_input_text_pos, font_size, spacing, GOLD);

        {
            Color backBtnColor = GetButtonColor(credential_back_posRec, mousepos, Button_color);
            DrawRectangleRounded(credential_back_posRec, 1.0f, 8, backBtnColor);
        }
        DrawTextEx(font_play, back_message, credential_back_pos, (float)font_play.baseSize + 30, 2, BLUE);

        int key = GetCharPressed();
        while (key > 0)
        {
            if (nameLetterCount < MAX_NAME_LEN)
            {
                playerName[nameLetterCount] = (char)key;
                nameLetterCount++;
                playerName[nameLetterCount] = '\0';
            }
            key = GetCharPressed();
        }

        // backspace
        if (IsKeyPressed(KEY_BACKSPACE) && nameLetterCount > 0)
        {
            nameLetterCount--;
            playerName[nameLetterCount] = '\0';
        }

        if (CheckCollisionPointRec(mousepos, credential_back_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            playerName[0] = '\0';
            nameLetterCount = 0;
            level = ZERO_WINDOW;
        }

        if (IsKeyPressed(KEY_ENTER) && nameLetterCount > 0)
        {
            PlaySound(clicksound);
            level = RULES;
        }

        break;
    }
    case RULES:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawRectangleRounded(rules_box, 0.1f, 8, (Color){10, 15, 40, 230});
        DrawRectangleRoundedLinesEx(rules_box, 0.1f, 8, 2, GOLD);

        snprintf(rules_line1, sizeof(rules_line1), "Hello %s,", playerName);

        DrawTextEx(font_play, rules_line1, rules_line_pos[0], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line2, rules_line_pos[1], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line3, rules_line_pos[2], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line4, rules_line_pos[3], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line5, rules_line_pos[4], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line6, rules_line_pos[5], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line7, rules_line_pos[6], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line8, rules_line_pos[7], 26, spacing, GOLD);
        DrawTextEx(font_play, rules_line9, rules_line_pos[8], 26, spacing, GOLD);

        {
            Color startBtnColor = GetButtonColor(start_button_posRec, mousepos, Button_color);
            DrawRectangleRounded(start_button_posRec, 1.0f, 8, startBtnColor);
            DrawRectangleRoundedLinesEx(start_button_posRec, 1.0f, 8, 2, BLACK);
        }
        DrawTextEx(font_play, start_button_message, start_button_pos, (float)font_play.baseSize + 20, 2, GREEN);

        if (CheckCollisionPointRec(mousepos, start_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            currentLevelNumber = 1;
            StartLevelTimer();
            level = LEVEL_1;
        }

        break;

    case LEVEL_1:

        mousepos = GetMousePosition();
        if (IsKeyPressed(KEY_SPACE))
        {
            gamePaused = !gamePaused;
            timerRunning = !gamePaused;
            PlaySound(clicksound);

            if (gamePaused)
                PauseMusicStream(backgrnd_music);
            else
                ResumeMusicStream(backgrnd_music);
        }

        if (timerRunning)
            levelElapsedTime += GetFrameTime();
        if (teleportCooldown > 0.0f)
            teleportCooldown -= GetFrameTime();
        blackholeAnimTime += GetFrameTime();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawPlanetCentered(planet, planet_position[0]);

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount1; i++)
        {
            DrawWall(wall_level1[i]);
        }

        if (!AllKeysCollected(keys_level1))
        {
            for (int i = 0; i < 4; i++)
                DrawWallThick(redWalls_level1[i], RED, PLANET_WALL_THICK);
        }

        DrawKeys(keys_level1, blackholeAnimTime);

        DrawBlackHole(blackholes_level1[0], blackholeAnimTime);
        DrawBlackHole(blackholes_level1[1], blackholeAnimTime);

        // UpdateAndDrawFlames(flameEmitters1, flameEmitterCount1, GetFrameTime());
        if (!gamePaused)
        {
            updateRocket(&rocket, wall_level1, wallCount1, redWalls_level1, !AllKeysCollected(keys_level1), NULL, 0);
            UpdateKeyPickups(keys_level1, rocket.pos);

            {
                Vector2 teleportDest;
                if (CheckBlackHoleTeleport(rocket.pos, blackholes_level1, &teleportDest))
                {
                    rocket.pos = teleportDest;
                    teleportCooldown = BLACKHOLE_COOLDOWN;
                    PlaySound(clicksound);
                }
            }
        }

        DrawHUD(1, KEY_TYPE_COUNT - CountKeysCollected(keys_level1));

        if (gamePaused)
        {
            HandlePauseMenu();
        }

        if (!gamePaused && AllKeysCollected(keys_level1) && is_at_same_place(planet_position[0], rocket.pos))
        {
            PlaySound(level_up_sound);
            StopLevelTimer(0);
            level = TR_WIN_1;
        }
        break;

    case TR_WIN_1:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background,
                       (Rectangle){0, 0, space_background.width, space_background.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        {
            Color nextBtnColor = GetButtonColor(message_box, mousepos, Button_color);
            DrawRectangleRounded(message_box, 1.0f, 8, nextBtnColor);
        }
        DrawRectangleRoundedLinesEx(message_box, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition_msg1, message1_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition_msg2, message2_pos, font_size, spacing, BLUE);

        {
            char yourTimeStr[32];
            char resultText[64];
            snprintf(yourTimeStr, sizeof(yourTimeStr), "%s", FormatTime(levelTimes[0]));
            snprintf(resultText, sizeof(resultText), "Your Time: %s   Best: %s", yourTimeStr, FormatTime(bestLevelTimes[0]));
            Vector2 resultSize = MeasureTextEx(font_play, resultText, 22, spacing);
            DrawTextEx(font_play, resultText, (Vector2){screen_width / 2 - resultSize.x / 2, message_box.y + message_box.height + 15}, 22, spacing, GOLD);
        }

        if (CheckCollisionPointRec(mousepos, message_box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            rocket.pos = rocket_position[1];
            rocket.dir = DOWN;
            currentLevelNumber = 2;
            StartLevelTimer();
            level = LEVEL_2;
        }

        break;

    case LEVEL_2:
        mousepos = GetMousePosition();
        if (IsKeyPressed(KEY_SPACE))
        {
            gamePaused = !gamePaused;
            timerRunning = !gamePaused;
            PlaySound(clicksound);

            if (gamePaused)
                PauseMusicStream(backgrnd_music);
            else
                ResumeMusicStream(backgrnd_music);
        }

        if (timerRunning)
            levelElapsedTime += GetFrameTime();
        if (teleportCooldown > 0.0f)
            teleportCooldown -= GetFrameTime();
        blackholeAnimTime += GetFrameTime();

        DrawTexturePro(space_background2,
                       (Rectangle){0, 0, space_background2.width, space_background2.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawPlanetCentered(planet, planet_position[1]);

        for (int i = 0; i < meteors_level2_size; i++)
        {
            DrawMeteorCentered(meteor, meteors_level2[i]);
        }

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount2; i++)
        {
            DrawWall(wall_level2[i]);
        }

        if (!AllKeysCollected(keys_level2))
        {
            for (int i = 0; i < 4; i++)
                DrawWallThick(redWalls_level2[i], RED, PLANET_WALL_THICK);
        }

        DrawKeys(keys_level2, blackholeAnimTime);

        DrawBlackHole(blackholes_level2[0], blackholeAnimTime);
        DrawBlackHole(blackholes_level2[1], blackholeAnimTime);

        // UpdateAndDrawFlames(flameEmitters2, flameEmitterCount2, GetFrameTime());

        if (!gamePaused)
        {
            updateRocket(&rocket, wall_level2, wallCount2, redWalls_level2, !AllKeysCollected(keys_level2), meteors_level2, meteors_level2_size);
            UpdateKeyPickups(keys_level2, rocket.pos);

            {
                Vector2 teleportDest;
                if (CheckBlackHoleTeleport(rocket.pos, blackholes_level2, &teleportDest))
                {
                    rocket.pos = teleportDest;
                    teleportCooldown = BLACKHOLE_COOLDOWN;
                    PlaySound(clicksound);
                }
            }
        }

        DrawHUD(2, KEY_TYPE_COUNT - CountKeysCollected(keys_level2));

        if (gamePaused)
        {
            HandlePauseMenu();
        }

        if (!gamePaused && AllKeysCollected(keys_level2) && is_at_same_place(planet_position[1], rocket.pos))
        {
            PlaySound(level_up_sound);
            StopLevelTimer(1);
            level = TR_WIN_2;
        }
        break;

    case TR_WIN_2:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background2,
                       (Rectangle){0, 0, space_background2.width, space_background2.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        {
            Color nextBtnColor = GetButtonColor(message_box2, mousepos, Button_color);
            DrawRectangleRounded(message_box2, 1.0f, 8, nextBtnColor);
        }

        DrawRectangleRoundedLinesEx(message_box2, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition2_msg1, message3_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition2_msg2, message4_pos, font_size, spacing, BLUE);

        {
            char yourTimeStr[32];
            char resultText[64];
            snprintf(yourTimeStr, sizeof(yourTimeStr), "%s", FormatTime(levelTimes[1]));
            snprintf(resultText, sizeof(resultText), "Your Time: %s   Best: %s", yourTimeStr, FormatTime(bestLevelTimes[1]));
            Vector2 resultSize = MeasureTextEx(font_play, resultText, 22, spacing);
            DrawTextEx(font_play, resultText, (Vector2){screen_width / 2 - resultSize.x / 2, message_box.y + message_box.height + 15}, 22, spacing, GOLD);
        }

        if (CheckCollisionPointRec(mousepos, message_box2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            rocket.pos = rocket_position[2];
            rocket.dir = RIGHT;
            currentLevelNumber = 3;
            StartLevelTimer();
            level = LEVEL_3;
        }

        break;

    case LEVEL_3:
        mousepos = GetMousePosition();
        if (IsKeyPressed(KEY_SPACE))
        {
            gamePaused = !gamePaused;
            timerRunning = !gamePaused;
            PlaySound(clicksound);

            if (gamePaused)
                PauseMusicStream(backgrnd_music);
            else
                ResumeMusicStream(backgrnd_music);
        }

        if (timerRunning)
            levelElapsedTime += GetFrameTime();
        if (teleportCooldown > 0.0f)
            teleportCooldown -= GetFrameTime();
        blackholeAnimTime += GetFrameTime();

        DrawTexturePro(space_background3,
                       (Rectangle){0, 0, space_background3.width, space_background3.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawPlanetCentered(planet, planet_position[2]);

        for (int i = 0; i < meteors_level3_size; i++)
        {
            DrawMeteorCentered(meteor, meteors_level3[i]);
        }

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount3; i++)
        {
            DrawWall(wall_level3[i]);
        }

        if (!AllKeysCollected(keys_level3))
        {
            for (int i = 0; i < 4; i++)
                DrawWallThick(redWalls_level3[i], RED, PLANET_WALL_THICK);
        }

        DrawKeys(keys_level3, blackholeAnimTime);

        DrawBlackHole(blackholes_level3[0], blackholeAnimTime);
        DrawBlackHole(blackholes_level3[1], blackholeAnimTime);

        //  UpdateAndDrawFlames(flameEmitters3, flameEmitterCount3, GetFrameTime());

        if (!gamePaused)
        {
            UpdateAndDrawFlames(flameEmitters3, flameEmitterCount3, GetFrameTime() * 0.8f);

            updateRocket(&rocket, wall_level3, wallCount3, redWalls_level3, !AllKeysCollected(keys_level3), meteors_level3, meteors_level3_size);
            UpdateKeyPickups(keys_level3, rocket.pos);

            {
                Vector2 teleportDest;
                if (CheckBlackHoleTeleport(rocket.pos, blackholes_level3, &teleportDest))
                {
                    rocket.pos = teleportDest;
                    teleportCooldown = BLACKHOLE_COOLDOWN;
                    PlaySound(clicksound);
                }
            }
        }
        else
        {
            UpdateAndDrawFlames(flameEmitters3, flameEmitterCount3, GetFrameTime() * 0.8f);
        }

        DrawHUD(3, KEY_TYPE_COUNT - CountKeysCollected(keys_level3));

        if (gamePaused)
        {
            HandlePauseMenu();
        }

        if (!gamePaused && AllKeysCollected(keys_level3) && is_at_same_place(planet_position[2], rocket.pos))
        {
            PlaySound(level_up_sound);
            StopLevelTimer(2);
            level = TR_WIN_3;
        }
        break;

    case TR_WIN_3:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background3,
                       (Rectangle){0, 0, space_background3.width, space_background3.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        {
            Color nextBtnColor = GetButtonColor(message_box3, mousepos, Button_color);
            DrawRectangleRounded(message_box3, 1.0f, 8, nextBtnColor);
        }

        DrawRectangleRoundedLinesEx(message_box3, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition3_msg1, message5_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition3_msg2, message6_pos, font_size, spacing, BLUE);

        {
            char yourTimeStr[32];
            char resultText[64];
            snprintf(yourTimeStr, sizeof(yourTimeStr), "%s", FormatTime(levelTimes[2]));
            snprintf(resultText, sizeof(resultText), "Your Time: %s   Best: %s", yourTimeStr, FormatTime(bestLevelTimes[2]));
            Vector2 resultSize = MeasureTextEx(font_play, resultText, 22, spacing);
            DrawTextEx(font_play, resultText, (Vector2){screen_width / 2 - resultSize.x / 2, message_box.y + message_box.height + 15}, 22, spacing, GOLD);
        }

        if (CheckCollisionPointRec(mousepos, message_box3) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            rocket.pos = rocket_position[3];
            rocket.dir = LEFT;
            currentLevelNumber = 4;
            StartLevelTimer();
            ResetSpaceShips();
            level = LEVEL4;
        }

        break;

    case LEVEL4:
        mousepos = GetMousePosition();
        if (IsKeyPressed(KEY_SPACE))
        {
            gamePaused = !gamePaused;
            timerRunning = !gamePaused;
            PlaySound(clicksound);

            if (gamePaused)
                PauseMusicStream(backgrnd_music);
            else
                ResumeMusicStream(backgrnd_music);
        }

        if (timerRunning)
            levelElapsedTime += GetFrameTime();
        if (teleportCooldown > 0.0f)
            teleportCooldown -= GetFrameTime();
        blackholeAnimTime += GetFrameTime();

        if (!gamePaused)
        {
            updateSpaceShip(&alienShip1);
            updateSpaceShip(&alienShip2);
        }

        DrawTexturePro(space_background4,
                       (Rectangle){0, 0, space_background4.width, space_background4.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawPlanetCentered(planet, planet_position[3]);
        DrawSpaceShipCentered(alien_spaceship, alienShip1.pos);
        DrawSpaceShipCentered(alien_spaceship, alienShip2.pos);

        DrawTexturePro(rocketTex[rocket.dir],
                       (Rectangle){0, 0, rocketTex[rocket.dir].width, rocketTex[rocket.dir].height},
                       (Rectangle){rocket.pos.x * CELL, rocket.pos.y * CELL, rocketSize, rocketSize},
                       (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < wallCount4; i++)
        {
            DrawWall(wall_level4[i]);
        }

        if (!AllKeysCollected(keys_level4))
        {
            for (int i = 0; i < 4; i++)
                DrawWallThick(redWalls_level4[i], RED, PLANET_WALL_THICK);
        }

        DrawKeys(keys_level4, blackholeAnimTime);

        if (!gamePaused)
        {
            UpdateAndDrawFlames(flameEmitters4, flameEmitterCount4, GetFrameTime() * 1.2f);

            updateRocket(&rocket, wall_level4, wallCount4, redWalls_level4, !AllKeysCollected(keys_level4), NULL, 0);
            UpdateKeyPickups(keys_level4, rocket.pos);

            if (CheckAlienShipCollision(rocket.pos, alienShip1.pos))
            {
                rocket.pos = rocket_position[3];
                PlaySound(crashsound);
            }
            if (CheckAlienShipCollision(rocket.pos, alienShip2.pos))
            {
                rocket.pos = rocket_position[3];
                PlaySound(crashsound);
            }

            {
                Vector2 teleportDest;
                if (CheckBlackHoleTeleport(rocket.pos, blackholes_level4, &teleportDest))
                {
                    rocket.pos = teleportDest;
                    teleportCooldown = BLACKHOLE_COOLDOWN;
                    PlaySound(clicksound);
                }
            }
        }
        else
        {
            UpdateAndDrawFlames(flameEmitters4, flameEmitterCount4, 0.0f);
        }

        DrawHUD(4, KEY_TYPE_COUNT - CountKeysCollected(keys_level4));

        if (gamePaused)
        {
            HandlePauseMenu();
        }

        if (!gamePaused && AllKeysCollected(keys_level4) && is_at_same_place(planet_position[3], rocket.pos))
        {
            PlaySound(level_up_sound);
            StopLevelTimer(3);
            level = TR_WIN_4;
        }
        break;

    case TR_WIN_4:
        mousepos = GetMousePosition();

        DrawTexturePro(space_background4,
                       (Rectangle){0, 0, space_background4.width, space_background4.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        {
            Color nextBtnColor = GetButtonColor(message_box3, mousepos, Button_color);
            DrawRectangleRounded(message_box3, 1.0f, 8, nextBtnColor);
        }

        DrawRectangleRoundedLinesEx(message_box3, 1.0f, 8, 2, BLACK);

        DrawTextEx(font_play, transition4_msg1, message5_pos, font_size, spacing, BLUE);
        DrawTextEx(font_play, transition4_msg2, message6_pos, font_size, spacing, BLUE);

        {
            char yourTimeStr[32];
            char resultText[64];
            snprintf(yourTimeStr, sizeof(yourTimeStr), "%s", FormatTime(levelTimes[3]));
            snprintf(resultText, sizeof(resultText), "Your Time: %s   Best: %s", yourTimeStr, FormatTime(bestLevelTimes[3]));
            Vector2 resultSize = MeasureTextEx(font_play, resultText, 22, spacing);
            DrawTextEx(font_play, resultText, (Vector2){screen_width / 2 - resultSize.x / 2, message_box.y + message_box.height + 15}, 22, spacing, GOLD);
        }

        if (CheckCollisionPointRec(mousepos, message_box3) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = TOTAL_WINDOW;
        }
        break;

    case TOTAL_WINDOW:

        mousepos = GetMousePosition();

        // Compute this run's average time and write it to the players
        // leaderboard file exactly once per completed run.
        if (!scoreSaved)
        {
            avgLevelTime = ComputeAverageTime();
            SavePlayerScore(playerName, avgLevelTime);
            scoreSaved = true;
        }

        snprintf(score_title, sizeof(score_title), "Score of %s", playerName);

        DrawTexturePro(space_background5,
                       (Rectangle){0, 0, space_background5.width, space_background5.height},
                       (Rectangle){0, 0, screen_width, screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTextEx(font_play, score_title, score_title_pos, (float)font_play.baseSize + 20, 2, GOLD);

        for (int i = 0; i < LEVEL_COUNT; i++)
        {
            char levelLabel[16];
            snprintf(levelLabel, sizeof(levelLabel), "Level %d", i + 1);
            DrawTextEx(font_play, levelLabel, level_label_pos[i], font_size, spacing, BLUE);
            DrawTextEx(font_play, FormatTime(levelTimes[i]), level_time_pos[i], font_size, spacing, (Color){0, 255, 180, 255});
        }

        DrawTextEx(font_play, "Score: ", avg_time_pos, font_size, spacing, GOLD);
        DrawTextEx(font_play, FormatTime(avgLevelTime),
                   (Vector2){avg_time_pos.x + MeasureTextEx(font_play, "Average", font_size, spacing).x + 45, avg_time_pos.y},
                   font_size, spacing, GOLD);

        {
            Color exitBtnColor = GetButtonColor(exit_button_posRec, mousepos, Button_color);
            Color playAgainBtnColor = GetButtonColor(playagain_button_posRec, mousepos, Button_color);

            DrawRectangleRounded(exit_button_posRec, 1.0f, 8, exitBtnColor);
            DrawRectangleRoundedLinesEx(exit_button_posRec, 1.0f, 8, 2, BLACK);
            DrawTextEx(font_play, exit_message, exit_button_pos, (float)font_play.baseSize + 20, 2, BLUE);

            DrawRectangleRounded(playagain_button_posRec, 1.0f, 8, playAgainBtnColor);
            DrawRectangleRoundedLinesEx(playagain_button_posRec, 1.0f, 8, 2, BLACK);
            DrawTextEx(font_play, play_again_message, playagain_button_pos, (float)font_play.baseSize + 20, 2, BLUE);
        }

        if (CheckCollisionPointRec(mousepos, exit_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            exitGameRequested = true;
        }

        if (CheckCollisionPointRec(mousepos, playagain_button_posRec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            PlaySound(clicksound);
            level = ZERO_WINDOW;
        }

        break;
    }
}