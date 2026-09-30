void load_data_0()
{
    float paddingX = 20;
    float paddingY = 15;

    font_play = LoadFont("D:/Maze-explorer/Fonts/ALIEN CYBERNETICS.ttf");
    play_button_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, play_message, (float)font_play.baseSize + 10, 2).x / 2,
                                screen_height / 2 - 30 - 90 - MeasureTextEx(font_play, play_message, (float)font_play.baseSize, 2).y / 2};

    game_title_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).x / 2,
                               screen_height / 10 - MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).y / 2};

    credentialButton_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, credential_title, (float)font_play.baseSize + 10, 2).x / 2,
                                     screen_height / 2 + 60 - 90 - MeasureTextEx(font_play, credential_title, (float)font_play.baseSize, 2).y / 2};

    credentialButton_posRec = (Rectangle){credentialButton_pos.x - paddingX,
                                          credentialButton_pos.y - paddingY,
                                          MeasureTextEx(font_play, credential_title, (float)font_play.baseSize + 10, 2).x + paddingX * 2,
                                          MeasureTextEx(font_play, credential_title, (float)font_play.baseSize + 10, 2).y + paddingY * 2};

    play_button_posRec = (Rectangle){play_button_pos.x - paddingX,
                                     play_button_pos.y - paddingY,
                                     MeasureTextEx(font_play, play_message, (float)font_play.baseSize + 10, 2).x + paddingX * 2,
                                     MeasureTextEx(font_play, play_message, (float)font_play.baseSize + 10, 2).y + paddingY * 2};

    game_title_posRec = (Rectangle){game_title_pos.x - paddingX,
                                    game_title_pos.y - paddingY,
                                    MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).x + paddingX * 2,
                                    MeasureTextEx(font_play, game_title, (float)font_play.baseSize + 40, 4).y / 2 + paddingY * 4};

    leaderboard_button_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, leaderboard_button_message, (float)font_play.baseSize + 10, 2).x / 2,
                                       credentialButton_posRec.y + credentialButton_posRec.height + 30};

    leaderboard_button_posRec = (Rectangle){leaderboard_button_pos.x - paddingX,
                                            leaderboard_button_pos.y - paddingY,
                                            MeasureTextEx(font_play, leaderboard_button_message, (float)font_play.baseSize + 10, 2).x + paddingX * 2,
                                            MeasureTextEx(font_play, leaderboard_button_message, (float)font_play.baseSize + 10, 2).y + paddingY * 2};

    howto_button_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, howto_button_message, (float)font_play.baseSize + 10, 2).x / 2,
                                 leaderboard_button_posRec.y + leaderboard_button_posRec.height + 30};

    howto_button_posRec = (Rectangle){howto_button_pos.x - paddingX,
                                      howto_button_pos.y - paddingY,
                                      MeasureTextEx(font_play, howto_button_message, (float)font_play.baseSize + 10, 2).x + paddingX * 2,
                                      MeasureTextEx(font_play, howto_button_message, (float)font_play.baseSize + 10, 2).y + paddingY * 2};

    main_exit_button_pos = (Vector2){screen_width / 2 - MeasureTextEx(font_play, exit_message, (float)font_play.baseSize + 10, 2).x / 2,
                                     howto_button_posRec.y + howto_button_posRec.height + 30};

    main_exit_button_posRec = (Rectangle){main_exit_button_pos.x - paddingX,
                                          main_exit_button_pos.y - paddingY,
                                          MeasureTextEx(font_play, exit_message, (float)font_play.baseSize + 10, 2).x + paddingX * 2,
                                          MeasureTextEx(font_play, exit_message, (float)font_play.baseSize + 10, 2).y + paddingY * 2};
}

void load_data_leaderboard_window()
{
    Vector2 title_size = MeasureTextEx(font_play, leaderboard_title, (float)font_play.baseSize + 20, 2);
    leaderboard_title_pos = (Vector2){screen_width / 2 - title_size.x / 2, screen_height / 10};

    leaderboard_back_pos = (Vector2){30, 30};
    Vector2 back_size = MeasureTextEx(font_play, leaderboard_back_message, (float)font_play.baseSize + 30, 2);

    float paddingX = 20;
    float paddingY = 15;
    leaderboard_back_posRec = (Rectangle){
        leaderboard_back_pos.x - paddingX,
        leaderboard_back_pos.y - paddingY,
        back_size.x + paddingX * 2,
        back_size.y + paddingY * 2};
}
void load_data_howto_window()
{
    Vector2 title_size = MeasureTextEx(font_play, howto_title, (float)font_play.baseSize + 20, 2);
    howto_title_pos = (Vector2){screen_width / 2 - title_size.x / 2, screen_height / 12};

    howto_back_pos = (Vector2){30, 30};
    Vector2 back_size = MeasureTextEx(font_play, back_message, (float)font_play.baseSize + 30, 2);
    howto_back_posRec = (Rectangle){
        howto_back_pos.x - 20,
        howto_back_pos.y - 15,
        back_size.x + 40,
        back_size.y + 30};

    howto_box = (Rectangle){screen_width / 2 - 600, 170, 1200, 620};

    for (int i = 0; i < HOWTO_LINE_COUNT; i++)
    {
        howto_line_pos[i] = (Vector2){howto_box.x + 40, howto_box.y + 45 + i * 65};
    }
}

void load_credentials()
{

    float line_gap = 20;
    float credSize = (float)font_play.baseSize + 10;

    Vector2 name1_size = MeasureTextEx(font_play, credential_name1, credSize, 2);
    Vector2 name2_size = MeasureTextEx(font_play, credential_name2, credSize, 2);
    Vector2 supervisor_label_size = MeasureTextEx(font_play, supervisor_label, credSize, 2);
    Vector2 supervisor_name_size = MeasureTextEx(font_play, supervisor_name, credSize, 2);

    float names_total_height = name1_size.y + name2_size.y + line_gap;

    credential_name1_pos = (Vector2){
        screen_width / 2 - name1_size.x / 2,
        screen_height / 2 - names_total_height / 2};

    credential_name2_pos = (Vector2){
        screen_width / 2 - name2_size.x / 2,
        credential_name1_pos.y + name1_size.y + line_gap};

    supervisor_label_pos = (Vector2){
        screen_width / 2 - supervisor_label_size.x / 2,
        credential_name2_pos.y + name2_size.y + line_gap * 2};

    supervisor_name_pos = (Vector2){
        screen_width / 2 - supervisor_name_size.x / 2,
        supervisor_label_pos.y + supervisor_label_size.y + line_gap};

    credential_back_pos = (Vector2){30, 30};

    Vector2 back_size = MeasureTextEx(font_play, back_message, (float)font_play.baseSize + 30, 2);

    float paddingX = 20;
    float paddingY = 15;

    credential_back_posRec = (Rectangle){
        credential_back_pos.x - paddingX,
        credential_back_pos.y - paddingY,
        back_size.x + paddingX * 2,
        back_size.y + paddingY * 2};

    float boxPadX = 60;
    float boxPadY = 40;

    float textLeft = fminf(fminf(credential_name1_pos.x, credential_name2_pos.x),
                           fminf(supervisor_label_pos.x, supervisor_name_pos.x));

    float textRight = fmaxf(fmaxf(credential_name1_pos.x + name1_size.x, credential_name2_pos.x + name2_size.x),
                            fmaxf(supervisor_label_pos.x + supervisor_label_size.x, supervisor_name_pos.x + supervisor_name_size.x));

    float textTop = credential_name1_pos.y;
    float textBottom = supervisor_name_pos.y + supervisor_name_size.y;

    credential_box = (Rectangle){
        textLeft - boxPadX,
        textTop - boxPadY,
        (textRight - textLeft) + boxPadX * 2,
        (textBottom - textTop) + boxPadY * 2};
}

void load_data_name_window()
{
    float boxHeight = 60;
    float labelBoxWidth = 120;
    float inputBoxWidth = 300;
    float gap = 10;

    float totalWidth = labelBoxWidth + gap + inputBoxWidth;
    float startX = screen_width / 2 - totalWidth / 2;
    float boxY = screen_height / 2 - boxHeight / 2;

    name_label_box = (Rectangle){startX, boxY, labelBoxWidth, boxHeight};
    name_input_box = (Rectangle){startX + labelBoxWidth + gap, boxY, inputBoxWidth, boxHeight};

    Vector2 label_size = MeasureTextEx(font_play, name_label, font_size, spacing);
    name_label_pos = (Vector2){
        name_label_box.x + name_label_box.width / 2 - label_size.x / 2,
        name_label_box.y + name_label_box.height / 2 - label_size.y / 2};

    name_input_text_pos = (Vector2){
        name_input_box.x + 15,
        name_input_box.y + name_input_box.height / 2 - font_size / 2};
}
void load_data_pause_window()
{
    float box_width = 500;
    float box_height = 360;

    pause_box = (Rectangle){
        screen_width / 2 - box_width / 2,
        screen_height / 2 - box_height / 2,
        box_width,
        box_height};

    Vector2 pause_msg_size = MeasureTextEx(font_play, pause_message, (float)font_play.baseSize + 20, 2);
    pause_message_pos = (Vector2){
        screen_width / 2 - pause_msg_size.x / 2,
        pause_box.y + 30};

    float paddingX = 30;
    float paddingY = 15;

    Vector2 resume_size = MeasureTextEx(font_play, resume_button_message, (float)font_play.baseSize + 10, 2);
    resume_button_pos = (Vector2){
        screen_width / 2 - resume_size.x / 2,
        pause_box.y + 120};
    resume_button_posRec = (Rectangle){
        resume_button_pos.x - paddingX,
        resume_button_pos.y - paddingY,
        resume_size.x + paddingX * 2,
        resume_size.y + paddingY * 2};

    Vector2 music_size = MeasureTextEx(font_play, music_off_message, (float)font_play.baseSize + 10, 2);
    music_button_posRec = (Rectangle){
        screen_width / 2 - music_size.x / 2 - paddingX,
        pause_box.y + 190 - paddingY,
        music_size.x + paddingX * 2,
        music_size.y + paddingY * 2};

    Vector2 exit_size = MeasureTextEx(font_play, exit_game_button_message, (float)font_play.baseSize + 10, 2);
    exit_game_button_pos = (Vector2){
        screen_width / 2 - exit_size.x / 2,
        pause_box.y + 260};
    exit_game_button_posRec = (Rectangle){
        exit_game_button_pos.x - paddingX,
        exit_game_button_pos.y - paddingY,
        exit_size.x + paddingX * 2,
        exit_size.y + paddingY * 2};
}
void load_data_rules_window()
{
    float box_width = 900;
    float box_height = 400;

    rules_box = (Rectangle){
        screen_width / 2 - box_width / 2,
        screen_height / 2 - box_height / 2 - 30,
        box_width,
        box_height};

    float line_gap = 34;
    float start_y = rules_box.y + 40;

    for (int i = 0; i < 9; i++)
    {
        rules_line_pos[i] = (Vector2){
            rules_box.x + 30,
            start_y + i * line_gap};
    }

    Vector2 start_size = MeasureTextEx(font_play, start_button_message, (float)font_play.baseSize + 20, 2);
    start_button_pos = (Vector2){
        screen_width / 2 - start_size.x / 2,
        rules_box.y + box_height + 40};

    float paddingX = 30;
    float paddingY = 15;
    start_button_posRec = (Rectangle){
        start_button_pos.x - paddingX,
        start_button_pos.y - paddingY,
        start_size.x + paddingX * 2,
        start_size.y + paddingY * 2};
}

void load_data_1()
{
    space_background = LoadTexture("D:/Maze-explorer/Background/2.png");
    space_background2 = LoadTexture("D:/Maze-explorer/Background/3.png");
    space_background3 = LoadTexture("D:/Maze-explorer/Background/4.png");
    space_background4 = LoadTexture("D:/Maze-explorer/Background/5.png");
    space_background5 = LoadTexture("D:/Maze-explorer/Background/6.png");
    planet = LoadTexture("D:/Maze-explorer/Planets/planet.png");
    meteor = LoadTexture("D:/Maze-explorer/Meteor/1.png");
    alien_spaceship = LoadTexture("D:/Maze-explorer/Alien-spaceship/1.png");
}

void load_data_transition_window()
{
    float padding_x = 50;
    float padding_y = 30;
    float line_gap = 20;

    Vector2 msg1_size = MeasureTextEx(font_play, transition_msg1, font_size, spacing);
    Vector2 msg2_size = MeasureTextEx(font_play, transition_msg2, font_size, spacing);

    float text_width = (msg1_size.x > msg2_size.x) ? msg1_size.x : msg2_size.x;

    float box_width = text_width + padding_x * 2;
    float box_height = msg1_size.y + msg2_size.y + line_gap + padding_y * 2;

    message_box = (Rectangle){
        screen_width / 2 - box_width / 2,
        screen_height / 2 - box_height / 2,
        box_width,
        box_height};

    message1_pos = (Vector2){
        screen_width / 2 - msg1_size.x / 2,
        message_box.y + padding_y};

    message2_pos = (Vector2){
        screen_width / 2 - msg2_size.x / 2,
        message_box.y + padding_y + msg1_size.y + line_gap};
}

void load_data_transition_window2()
{
    float padding_x = 50;
    float padding_y = 30;
    float line_gap = 20;

    Vector2 msg1_size = MeasureTextEx(font_play, transition2_msg1, font_size, spacing);
    Vector2 msg2_size = MeasureTextEx(font_play, transition2_msg2, font_size, spacing);

    float text_width = (msg1_size.x > msg2_size.x) ? msg1_size.x : msg2_size.x;

    float box_width = text_width + padding_x * 2;
    float box_height = msg1_size.y + msg2_size.y + line_gap + padding_y * 2;

    message_box2 = (Rectangle){
        screen_width / 2 - box_width / 2,
        screen_height / 2 - box_height / 2,
        box_width,
        box_height};

    message3_pos = (Vector2){
        screen_width / 2 - msg1_size.x / 2,
        message_box2.y + padding_y};

    message4_pos = (Vector2){
        screen_width / 2 - msg2_size.x / 2,
        message_box2.y + padding_y + msg1_size.y + line_gap};
}

void load_data_transition_window3()
{
    float padding_x = 50;
    float padding_y = 30;
    float line_gap = 20;

    Vector2 msg1_size = MeasureTextEx(font_play, transition3_msg1, font_size, spacing);
    Vector2 msg2_size = MeasureTextEx(font_play, transition3_msg2, font_size, spacing);

    float text_width = (msg1_size.x > msg2_size.x) ? msg1_size.x : msg2_size.x;

    float box_width = text_width + padding_x * 2;
    float box_height = msg1_size.y + msg2_size.y + line_gap + padding_y * 2;

    message_box3 = (Rectangle){
        screen_width / 2 - box_width / 2,
        screen_height / 2 - box_height / 2,
        box_width,
        box_height};

    message5_pos = (Vector2){
        screen_width / 2 - msg1_size.x / 2,
        message_box3.y + padding_y};

    message6_pos = (Vector2){
        screen_width / 2 - msg2_size.x / 2,
        message_box3.y + padding_y + msg1_size.y + line_gap};
}

void load_data_total_window()
{
    float padding_x = 50;
    float padding_y = 30;
    float line_gap = 15;
    float label_to_time_gap = 45;

    snprintf(score_title, sizeof(score_title), "Score of %s", playerName);
    Vector2 title_size = MeasureTextEx(font_play, score_title, (float)font_play.baseSize + 20, 2);
    score_title_pos = (Vector2){
        screen_width / 2 - title_size.x / 2 - 120,
        screen_height / 10};

    float rows_start_y = score_title_pos.y + title_size.y + 60;
    float row_start_x = screen_width / 2 - 150;

    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        char levelLabel[16];
        snprintf(levelLabel, sizeof(levelLabel), "Level %d", i + 1);
        Vector2 label_size = MeasureTextEx(font_play, levelLabel, font_size, spacing);

        level_label_pos[i] = (Vector2){
            row_start_x,
            rows_start_y + i * (label_size.y + line_gap)};

        level_time_pos[i] = (Vector2){
            row_start_x + label_size.x + label_to_time_gap,
            level_label_pos[i].y};
    }

    avg_time_pos = (Vector2){
        row_start_x,
        level_label_pos[LEVEL_COUNT - 1].y + (font_size + line_gap) + 20};

    Vector2 exit_size = MeasureTextEx(font_play, exit_message, (float)font_play.baseSize + 20, 2);
    exit_button_pos = (Vector2){
        screen_width / 2 - 250 - exit_size.x / 2,
        screen_height - 120};

    exit_button_posRec = (Rectangle){
        exit_button_pos.x - padding_x,
        exit_button_pos.y - padding_y,
        exit_size.x + padding_x * 2,
        exit_size.y + padding_y * 2};

    Vector2 playagain_size = MeasureTextEx(font_play, play_again_message, (float)font_play.baseSize + 20, 2);
    playagain_button_pos = (Vector2){
        screen_width / 2 + 250 - playagain_size.x / 2,
        screen_height - 120};

    playagain_button_posRec = (Rectangle){
        playagain_button_pos.x - padding_x,
        playagain_button_pos.y - padding_y,
        playagain_size.x + padding_x * 2,
        playagain_size.y + padding_y * 2};
}