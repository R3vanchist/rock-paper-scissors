#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 100

// Map selection
const char* get_choice_string(int my_selection) {
    switch (my_selection) {
        case 1:
            return "Rock";
        case 2:
            return "Paper";
        case 3:
            return "Scissors";
        default:
            return "Invalid selection"; 
    }
}

// Ask for the username
char *ask_username() {
    char *username = malloc(MAX_LINE_LENGTH); 
    if (!username) {
        perror("Memory allocation failed");
        exit(1); 
    }
    printf("What is your name?\n");
    scanf("%99s", username); 
    printf("Hello, %s\n", username);
    return username;
}

// Check for user in users.txt
int check_for_user(const char *filename, const char *username, int *wins, int *losses) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        // If the file does not exist, we return 0 (user not found)
        return 0;
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char stored_username[MAX_LINE_LENGTH];
        int stored_wins, stored_losses;
        sscanf(line, "%s %d-%d", stored_username, &stored_wins, &stored_losses);

        if (strcmp(stored_username, username) == 0) {
            *wins = stored_wins;
            *losses = stored_losses;
            fclose(file);
            return 1;  // Name found
        }
    }
    fclose(file);
    return 0;  // Name not found
}

// Function to write user's name and record to users.txt
int write_record_to_file(const char *filename, const char *username, int wins, int losses) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file.");
        return -1;
    }
    fprintf(file, "%s %d-%d\n", username, wins, losses);  // Store wins and losses along with username
    fclose(file); 
    return 0;
}

// Function to update the user's record in users.txt
int update_record_in_file(const char *filename, const char *username, int wins, int losses) {
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        // If the file does not exist, create it by opening it in write mode
        file = fopen(filename, "w");
        if (file == NULL) {
            perror("Error opening file.");
            return -1;
        }
    }

    char line[MAX_LINE_LENGTH];
    long pos;
    int found = 0;

    // Go through the file and find the user's record
    while ((pos = ftell(file)) != -1 && fgets(line, sizeof(line), file)) {
        char stored_username[MAX_LINE_LENGTH];
        int stored_wins, stored_losses;
        sscanf(line, "%s %d-%d", stored_username, &stored_wins, &stored_losses);

        if (strcmp(stored_username, username) == 0) {
            // We found the record, update it
            fseek(file, pos, SEEK_SET);
            fprintf(file, "%s %d-%d\n", username, wins, losses);
            found = 1;
            break;
        }
    }
    
    fclose(file);
    
    if (!found) {
        // If the user wasn't found, append a new record
        return write_record_to_file(filename, username, wins, losses);
    }
    return 0;
}

// Main game loop
int main() {
    int selection;
    int my_values[] = {1, 2, 3};
    int size = sizeof(my_values) / sizeof(my_values[0]);

    int wins = 0;
    int losses = 0;

    const char *filename = "users.txt";

    char *username = ask_username();

    // Check if the user exists and load their record if they do
    if (check_for_user(filename, username, &wins, &losses) == 1) {
        printf("Welcome back %s! Your current record is %d - %d.\n", username, wins, losses);
    } else {
        printf("Welcome, new user %s!\n", username);
    }

    char play_again = 'y';

    // Game loop (continue playing as long as user wants)
    do {
        printf("\nPlease make a selection: \n1 for Rock \n2 for Paper \n3 for Scissors\n");
        scanf("%d", &selection);

        // Validate user input
        if (selection < 1 || selection > 3) {
            printf("Please choose a valid number (1, 2, or 3)\n");
            continue;  
        }

        // Show user's choice
        switch(selection) {
            case 1:
                printf("You choose: Rock\n");
                break;
            case 2:
                printf("You choose: Paper\n");
                break;
            case 3:
                printf("You choose: Scissors\n");
                break;
        }

        // Randomly select the computer's choice
        srand(time(NULL));
        int randomIndex = rand() % size;
        int my_selection = my_values[randomIndex];
        printf("I choose: %s\n", get_choice_string(my_selection));

        // Game logic: Determine the winner
        if (selection == my_selection) {
            printf("It's a tie!\n");
            printf("Record: %d - %d\n", wins, losses);
        } 
        else if (selection == 1 && my_selection == 2) {
            printf("I win!\n");
            losses++;
            printf("Record: %d - %d\n", wins, losses);
        }
        else if (selection == 1 && my_selection == 3) {
            printf("You win!\n");
            wins++;
            printf("Record: %d - %d\n", wins, losses);
        }
        else if (selection == 2 && my_selection == 1) {
            printf("You win!\n");
            wins++;
            printf("Record: %d - %d\n", wins, losses);
        }
        else if (selection == 2 && my_selection == 3) {
            printf("I win!\n");
            losses++;
            printf("Record: %d - %d\n", wins, losses);
        }
        else if (selection == 3 && my_selection == 1) {
            printf("I win!\n");
            losses++;
            printf("Record: %d - %d\n", wins, losses);
        }
        else if (selection == 3 && my_selection == 2) {
            printf("You win!\n");
            wins++;
            printf("Record: %d - %d\n", wins, losses);
        }

        // Ask if the user wants to play again
        printf("\nDo you want to play again? (y/n): ");
        scanf(" %c", &play_again);  

    } while (play_again == 'y' || play_again == 'Y');

    printf("Thanks for playing %s! Your final record was %d - %d.\n", username, wins, losses);

    // Update the user's record in the file
    update_record_in_file(filename, username, wins, losses);

    free(username);  
    return 0;
}
