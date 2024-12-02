#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROOMS 10   // Maximum number of rooms
#define MAX_DAYS 7     // Seven days in a week (Monday to Sunday)

// Struct definition for storing study group information
typedef struct {
    int start;       // Study start time
    int end;         // Study end time
    char groupName;  // Group name (single character)
    char day[10];    // Day of the week (e.g., "Mon", "Tue")
    int room;        // Assigned room number (between 1 and 10)
} StudyGroup;

// Comparison function for sorting groups by their end time
// Used by qsort to sort groups in ascending order of end time
int compare(const void* a, const void* b) {
    StudyGroup* groupA = (StudyGroup*)a;
    StudyGroup* groupB = (StudyGroup*)b;
    return groupA->end - groupB->end;  // Compare end times (positive if groupA ends later)
}

// Function to check if a room is available for a given time period
// Ensures that the requested time does not overlap with other groups in the same room on the same day
// Returns 1 if available, 0 otherwise
int isAvailable(StudyGroup schedule[MAX_DAYS][MAX_ROOMS][100], int dayIdx, int room, int start, int end, int groupCount[MAX_DAYS][MAX_ROOMS]) {
    for (int i = 0; i < groupCount[dayIdx][room]; i++) {
        // Check for overlap (end time is before another group's start or start time is after another group's end)
        if (!(end <= schedule[dayIdx][room][i].start || start >= schedule[dayIdx][room][i].end)) {
            return 0;  // Return 0 if there is an overlap
        }
    }
    return 1;  // Return 1 if no overlap
}

// Function to schedule study groups using a greedy algorithm
// Attempts to assign groups to available rooms based on the earliest end time
void scheduleGroups(StudyGroup groups[], int n) {
    // Sort groups by end time (greedy approach)
    qsort(groups, n, sizeof(StudyGroup), compare);  // Sort groups using qsort

    // Array to store schedules for each day and room
    StudyGroup schedule[MAX_DAYS][MAX_ROOMS][100] = { {{0}} };
    // Array to record the number of groups scheduled for each day and room
    int groupCount[MAX_DAYS][MAX_ROOMS] = { {0} };

    // Array of day names (used for scheduling)
    const char* days[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
    printf("Scheduled study groups:\n");

    // Try to schedule each group
    for (int i = 0; i < n; i++) {
        int scheduled = 0;  // Flag to indicate if the group has been scheduled
        int dayIdx = -1;    // Index of the day (0: Monday, 6: Sunday)

        // Find the index of the requested day
        for (int d = 0; d < MAX_DAYS; d++) {
            if (strcmp(groups[i].day, days[d]) == 0) {
                dayIdx = d;
                break;
            }
        }

        // If the day is invalid, print an error message
        if (dayIdx == -1) {
            printf("Error: Group %c has an invalid day '%s'. Skipping scheduling.\n", groups[i].groupName, groups[i].day);
            continue;  // Skip to the next group
        }

        // Check each room sequentially for availability
        for (int room = 1; room <= MAX_ROOMS; room++) {
            // Check if the requested time is available in the room
            if (isAvailable(schedule, dayIdx, room, groups[i].start, groups[i].end, groupCount)) {
                // If available, assign the room to the group
                groups[i].room = room;

                // Add the group to the schedule for the day and room
                schedule[dayIdx][room][groupCount[dayIdx][room]] = groups[i];
                groupCount[dayIdx][room]++;  // Increment the number of groups in the room

                // Print a success message
                printf("Group %c: %s from %d to %d in room %d\n", groups[i].groupName, groups[i].day, groups[i].start, groups[i].end, room);
                scheduled = 1;  // Mark the group as scheduled
                break;
            }
        }

        // If all rooms are full, print a detailed failure message
        if (!scheduled) {
            printf("Error: Group %c could not be scheduled on %s from %d to %d.\n", groups[i].groupName, groups[i].day, groups[i].start, groups[i].end);
            printf("Reason: All %d rooms are fully booked during the requested time slot.\n", MAX_ROOMS);

            // Suggest possible alternatives
            printf("Suggestion: Try rescheduling the group to a different time or day.\n");
        }
    }
}

// Function to read group information from a file into an array
// Reads the number of groups and their details from input.txt
StudyGroup* readGroupsFromFile(const char* filename, int* n) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "r");  // Open the file
    if (err != 0 || file == NULL) {
        printf("File opening error!\n");  // Print error if the file cannot be opened
        exit(1);  // Exit the program
    }

    fscanf_s(file, "%d", n);  // Read the number of groups
    // Dynamically allocate an array of StudyGroup structs
    StudyGroup* groups = (StudyGroup*)malloc(*n * sizeof(StudyGroup));

    // Read each group's details (group name, day, start time, end time) from the file
    for (int i = 0; i < *n; i++) {
        fscanf_s(file, " %c %s %d %d", &groups[i].groupName, 1, groups[i].day, 10, &groups[i].start, &groups[i].end);
    }
    fclose(file);  // Close the file
    return groups;  // Return the array of groups
}

// Main function: Reads group information from a file, schedules them, and prints the results
int main() {
    int n;
    StudyGroup* groups = readGroupsFromFile("input.txt", &n);  // Read group information from the file
    scheduleGroups(groups, n);  // Schedule the groups
    free(groups);  // Free dynamically allocated memory
    return 0;
}
