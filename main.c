#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 30
#define MAX_GAMES_PER_USER 20
#define MAX_NAME_LENGTH 40
#define MAX_FILE_NAME_LENGTH 40
#define MAX_LINE_LENGTH 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    int rating;
} Game;

typedef struct {
    char username[MAX_NAME_LENGTH];
    Game games[MAX_GAMES_PER_USER];
    int num_games;
} User;

typedef struct {
    User users[MAX_USERS];
    int num_users;
    char filename[MAX_FILE_NAME_LENGTH];
} BoardGameRatings;

void initialize_board_game_ratings(BoardGameRatings *boardGameRatings, const char *filename) {
    boardGameRatings->num_users = 0;
    strcpy(boardGameRatings->filename, filename);
}

void read_data_from_file(BoardGameRatings *boardGameRatings) {
    FILE *file = fopen(boardGameRatings->filename, "r");
    if (file == NULL) {
        printf("File does not exist.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    User *current_user = NULL;

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0) {
            continue;
        }

       
        if (strchr(line, ' ') == NULL) {
            if (boardGameRatings->num_users >= MAX_USERS) {
                printf("Max number of users reached. Cannot add more users.\n");
                break;
            }
            strcpy(boardGameRatings->users[boardGameRatings->num_users].username, line);
            boardGameRatings->users[boardGameRatings->num_users].num_games = 0;
            current_user = &boardGameRatings->users[boardGameRatings->num_users];
            boardGameRatings->num_users++;
        } else {
            if (current_user == NULL) {
                printf("Error: game entry without a user.\n");
                continue;
            }
            if (current_user->num_games >= MAX_GAMES_PER_USER) {
                printf("Max number of games reached for user %s. Cannot add more games.\n", current_user->username);
                continue;
            }
            char game_name[MAX_NAME_LENGTH];
            int rating;
            sscanf(line, "%s %d", game_name, &rating);
            strcpy(current_user->games[current_user->num_games].name, game_name);
            current_user->games[current_user->num_games].rating = rating;
            current_user->num_games++;
        }
    }

    fclose(file);
}

void write_data_to_file(BoardGameRatings *boardGameRatings) {
    FILE *file = fopen(boardGameRatings->filename, "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
     for (int i = 0; i < boardGameRatings->num_users; i++) {
        fprintf(file, "%s\n", boardGameRatings->users[i].username);  
        for (int j = 0; j < boardGameRatings->users[i].num_games; j++) {
            fprintf(file, "%s %d\n", boardGameRatings->users[i].games[j].name, boardGameRatings->users[i].games[j].rating);  
        }
        if (i < boardGameRatings->num_users - 1) {
            fprintf(file, "\n");  
        }
    }
    fclose(file);
}

int find_user_index(BoardGameRatings *boardGameRatings, const char *username) {
    for (int i = 0; i < boardGameRatings->num_users; i++) {
        if (strcmp(boardGameRatings->users[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

void search_game(User *user);
void remove_game(User *user);
void print_user_games(User *user);
void add_game_to_user(User *user);

void user_menu(BoardGameRatings *boardGameRatings, User *user) {
    while (1) {
        printf("\n%s's board games\n", user->username);
        printf("1) Print games\n");
        printf("2) Add game\n");
        printf("3) Search games\n");
        printf("4) Remove game\n");
        printf("5) Exit\n");
        printf("Select: ");

        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
            print_user_games(user);
        } else if (choice == 2) {
            add_game_to_user(user);
        } else if (choice == 3) {
            search_game(user);
              
        } else if (choice == 4) {
             remove_game(user);
        } else if (choice == 5) {
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }
}

void print_user_games(User *user) {
    printf("%s\n", user->username);
    if (user->num_games == 0) {
        printf("          No games registered\n");
        return;
    }
    for (int i = 0; i < user->num_games; i++) {
        printf("    %-20s %8d\n", user->games[i].name, user->games[i].rating);
    }
}

void add_game_to_user(User *user) {
    while (user->num_games < MAX_GAMES_PER_USER) {
        char game_name[MAX_NAME_LENGTH];
        int rating;
        
        printf("Register new boardgame (q to quit): ");
        scanf("%s", game_name);
        
        if (strcmp(game_name, "q") == 0) break;

        int game_exists = 0;
        for (int i = 0; i < user->num_games; i++) {
            if (strcmp(user->games[i].name, game_name) == 0) {
                game_exists = 1;
                break;
            }
        }

        if (game_exists) {
            printf("Board game already added. Please add a different game.\n");
            continue;
        }

        while (1) {
            printf("Rating (1-10): ");
            scanf("%d", &rating);
            if (rating >= 1 && rating <= 10) break;
            printf("Illegal value!\n");
        }

        strcpy(user->games[user->num_games].name, game_name);
        user->games[user->num_games].rating = rating;
        user->num_games++;
    }
    if(user->num_games >= MAX_GAMES_PER_USER) {
        printf("Your game register is full.\n");
    }
}




void search_game(User *user) {
    while (1) {
        char search_term[MAX_NAME_LENGTH];
        printf("Search (q to quit): ");
        scanf("%s", search_term);
        if (strcmp(search_term, "q") == 0) break;

        printf("\nSearch results:\n");

        int num_matches = 0;
        for (int i = 0; i < user->num_games; i++) {
            if (strstr(user->games[i].name, search_term) != NULL) {
                printf("    %-20s %8d\n", user->games[i].name, user->games[i].rating);
                num_matches++;
            }
        }

        if (num_matches == 0) {
            printf("No matching games found.\n");
        } else {
            printf("Search complete. Use a more specific term to refine your search if needed.\n");
            return;
        }
    }
}

void remove_game(User *user) {
    if (user->num_games == 0) {
        printf("No games registered.\n");
        return;
    }

    while (1) {
        char search_term[MAX_NAME_LENGTH];
        printf("Search boardgame to remove (q to quit): ");
        scanf("%s", search_term);
        if (strcmp(search_term, "q") == 0) return;

        int found_indices[MAX_GAMES_PER_USER];
        int found_count = 0;
        for (int i = 0; i < user->num_games; i++) {
            if (strstr(user->games[i].name, search_term) != NULL) {
                found_indices[found_count++] = i;
            }
        }

        if (found_count == 0) {
            printf("No matching board game found.\n");
        } else if (found_count == 1) {
            int found_index = found_indices[0];
            printf("    %-20s %8d\n", user->games[found_index].name, user->games[found_index].rating);
            printf("Do you want to remove this game (y/n): ");
            char confirmation;
            scanf(" %c", &confirmation);
            if (confirmation == 'y' || confirmation == 'Y') {
                for (int i = found_index; i < user->num_games - 1; i++) {
                    user->games[i] = user->games[i + 1];
                }
                user->num_games--;
                printf("Game removed.\n");

                // Ask if the user wants to remove another game
                printf("Do you want to remove another game (y/n): ");
                scanf(" %c", &confirmation);
                if (confirmation == 'n' || confirmation == 'N') {
                    return;
                }
            } else {
                printf("Removal cancelled.\n");
                return;
            }
        } else {
            printf("Multiple matches found:\n");
            for (int i = 0; i < found_count; i++) {
                printf("    %-20s %8d\n", user->games[found_indices[i]].name, user->games[found_indices[i]].rating);
            }

            printf("Please refine your search to specify which game to remove.\n");
            printf("Search within these results (q to quit): ");
            scanf("%s", search_term);
            if (strcmp(search_term, "q") == 0) return;

            int refined_found_indices[MAX_GAMES_PER_USER];
            int refined_found_count = 0;
            for (int i = 0; i < found_count; i++) {
                if (strstr(user->games[found_indices[i]].name, search_term) != NULL) {
                    refined_found_indices[refined_found_count++] = found_indices[i];
                }
            }

            if (refined_found_count == 0) {
                printf("No matching board game found.\n");
            } else if (refined_found_count == 1) {
                int found_index = refined_found_indices[0];
                printf("%s: %d\n", user->games[found_index].name, user->games[found_index].rating);
                printf("Do you want to remove this game (y/n): ");
                char confirmation;
                scanf(" %c", &confirmation);
                if (confirmation == 'y' || confirmation == 'Y') {
                    for (int i = found_index; i < user->num_games - 1; i++) {
                        user->games[i] = user->games[i + 1];
                    }
                    user->num_games--;
                    printf("Game removed.\n");

                    // Ask if the user wants to remove another game
                    printf("Do you want to remove another game (y/n): ");
                    scanf(" %c", &confirmation);
                    if (confirmation == 'n' || confirmation == 'N') {
                        return;
                    }
                } else {
                    printf("Removal cancelled.\n");
                    return;
                }
            } else {
                printf("Multiple matches found again. Please refine your search further.\n");
            }
        }
    }
}




void start_menu(BoardGameRatings *boardGameRatings);

void admin_menu(BoardGameRatings *boardGameRatings);

void add_user(BoardGameRatings *boardGameRatings);

void remove_user(BoardGameRatings *boardGameRatings);

void print_all_users(BoardGameRatings *boardGameRatings);

void print_all_users_and_ratings(BoardGameRatings *boardGameRatings);

void print_games(User *user);


int main() {
    printf("Welcome to board game ratings.\n");

    BoardGameRatings boardGameRatings;
    char filename[MAX_FILE_NAME_LENGTH];

    printf("Enter the file name: ");
    scanf("%s", filename);

    initialize_board_game_ratings(&boardGameRatings, filename);

    read_data_from_file(&boardGameRatings);

    start_menu(&boardGameRatings);

    write_data_to_file(&boardGameRatings);

    printf("Goodbye!\n");

    return 0;
}

void start_menu(BoardGameRatings *boardGameRatings) {
    char choice[MAX_NAME_LENGTH];

    while (1) {
        printf("Please enter username, admin, or quit: ");
        scanf("%s", choice);

        if (strcmp(choice, "admin") == 0) {
            admin_menu(boardGameRatings);
        } else if (strcmp(choice, "quit") == 0) {
            write_data_to_file(boardGameRatings);
            break;
        } else {
            int user_index = find_user_index(boardGameRatings, choice);
            if (user_index != -1) {
                user_menu(boardGameRatings, &boardGameRatings->users[user_index]);
            } else {
                printf("User does not exist.\n");
            }
        }
    }
}

void admin_menu(BoardGameRatings *boardGameRatings) {
    int admin_choice;
    while (1) {
        printf("\nAdministration\n");
        printf("Choose:\n");
        printf("1) Add user\n");
        printf("2) Remove user\n");
        printf("3) Print all users\n");
        printf("4) Print all users and all their ratings\n");
        printf("5) Exit\n");

        printf("Please enter your choice: ");
        scanf("%d", &admin_choice);

        switch (admin_choice) {
            case 1:
                add_user(boardGameRatings);
                break;
            case 2:
                remove_user(boardGameRatings);
                break;
            case 3:
                print_all_users(boardGameRatings);
                break;
            case 4:
                print_all_users_and_ratings(boardGameRatings);
                break;
            case 5:
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void add_user(BoardGameRatings *boardGameRatings) {
    char new_username[MAX_NAME_LENGTH];

    while (1) {
        printf("Register new user (q to quit): ");
        scanf("%s", new_username);

        if (strcmp(new_username, "q") == 0) {
            return;
        }

        int user_exists = 0;
        for (int i = 0; i < boardGameRatings->num_users; i++) {
            if (strcmp(boardGameRatings->users[i].username, new_username) == 0) {
                user_exists = 1;
                break;
            }
        }

        if (user_exists) {
            printf("Username exists! Please choose another.\n");
        } else if (boardGameRatings->num_users >= MAX_USERS) {
            printf("The register is full!\n");
            return;
        } else {
            strcpy(boardGameRatings->users[boardGameRatings->num_users].username, new_username);
            boardGameRatings->users[boardGameRatings->num_users].num_games = 0;
            boardGameRatings->num_users++;
            printf("User added successfully.\n");
            return;
        }
    }
}

void remove_user(BoardGameRatings *boardGameRatings) {
    char user_to_remove[MAX_NAME_LENGTH];
    char confirm;

    while (1) {
        printf("Remove user (q to quit): ");
        scanf("%s", user_to_remove);

        if (strcmp(user_to_remove, "q") == 0) {
            return;
        }

        int user_index = -1;
        for (int i = 0; i < boardGameRatings->num_users; i++) {
            if (strcmp(boardGameRatings->users[i].username, user_to_remove) == 0) {
                user_index = i;
                break;
            }
        }

        if (user_index == -1) {
            printf("User does not exist! Please choose another.\n");
        } else {
            printf("Warning: User has rated games.\n");
            printf("Do you still want to remove %s (y/n)? ", user_to_remove);
            scanf(" %c", &confirm);

            if (confirm == 'y' || confirm == 'Y') {
                for (int i = user_index; i < boardGameRatings->num_users - 1; i++) {
                    strcpy(boardGameRatings->users[i].username, boardGameRatings->users[i + 1].username);
                    boardGameRatings->users[i].num_games = boardGameRatings->users[i + 1].num_games;
                    for (int j = 0; j < boardGameRatings->users[i + 1].num_games; j++) {
                        strcpy(boardGameRatings->users[i].games[j].name, boardGameRatings->users[i + 1].games[j].name);
                        boardGameRatings->users[i].games[j].rating = boardGameRatings->users[i + 1].games[j].rating;
                    }
                }
                boardGameRatings->num_users--;
                printf("Removed.\n");
            } else {
                printf("Operation canceled.\n");
            }

            return;
        }
    }
}

void print_all_users(BoardGameRatings *boardGameRatings) {
    if (boardGameRatings->num_users == 0) {
        printf("No users registered.\n");
        return;
    }

    printf("Users:\n");
    printf("________________________________________\n");
    for (int i = 0; i < boardGameRatings->num_users; i++) {
        printf("%s\n", boardGameRatings->users[i].username);
    }
}

void print_all_users_and_ratings(BoardGameRatings *boardGameRatings) {
    if (boardGameRatings->num_users == 0) {
        printf("No users registered.\n");
        return;
    }

    printf("Users and board games:\n");

    printf("+----------------------+----------------------+----------+\n");
    printf("|       Username       |         Game         |  Rating  |\n");
    printf("+----------------------+----------------------+----------+\n");

    for (int i = 0; i < boardGameRatings->num_users; i++) {
        printf("| %-20s |", boardGameRatings->users[i].username);

        if (boardGameRatings->users[i].num_games == 0) {
            printf(" %-20s | %8s |\n", "No games registered", "");
        } else {
            printf(" %-20s | %8d |\n", boardGameRatings->users[i].games[0].name, boardGameRatings->users[i].games[0].rating);
            for (int j = 1; j < boardGameRatings->users[i].num_games; j++) {
                printf("|                      | %-20s | %8d |\n", boardGameRatings->users[i].games[j].name, boardGameRatings->users[i].games[j].rating);
            }
        }
        printf("+----------------------+----------------------+----------+\n");
    }
}




void print_games(User *user) {
    if (user->num_games == 0) {
        printf("No games registered\n");
        return;
    }
    printf("Games for %s:\n", user->username);
    for (int i = 0; i < user->num_games; i++) {
        printf("%s: %d\n", user->games[i].name, user->games[i].rating);
    }
}


