/*
 * Copyright (c) 2021, suncloudsmoon and the Encrypt On The Go contributors.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>
#include <dir.h>
#include <windows.h>

#include "include/leafgame2d/leafgame2d.h"

#define APPNAME "Happy Secure"
#define CONTEXTMENUITEM "Encrypt/Decrypt"
#define REGFILECHECK "IsRegOk"
#define PATHSIZE 2048

/// TODO: Put the app struct and others in a different file, like app.h and app.c
typedef struct {
    char *homePath;
    char *logPath;
    char *regPath;
    char *appLocation;
    char *encryptKey; // Max 8 characters; Display that in the input dialog box

    FILE *plainText;
} App;

// Global Prototypes
void initApp();
void setupHomePath(char *appName, App *app);
void setupLogStuff(App *app);
void setupRegistry(App *app);
void freeApp(App *app);

// Static Prototypes


int main(char *fileLocation) {
    /// TODO: Hide the console based on the stuff in fileLocation

    App *app = (App *) calloc(1, sizeof(App));

    setupHomePath(APPNAME, app);

    if (fileLocation == NULL) {
        // Set up the reg stuff
        // Put up a messagebox dialog with 5 second delay for the next one that notifies the user that the app is installing and its done

    } else {
        // Check if the first character of the stream is E with \n following (actually read both of them); if not, then ask to encrypt
        // via messagebox with password as input

        // First: open the fileLocation
        char *encryptionStatus = readFileLocation(fileLocation, app);
        if (strcmp(encryptionStatus, "E\n")) {
            // Decrypt it

        } else {
            // Encrypt it
            // Reset the position of the file pointer back to zero (Hope it works)
            fseek(app->plainText, -2, SEEK_SET);

        }
    }

    freeApp(app);

    return 0;
}

char* readFileLocation(char *fileLocation, App *app) {
    if (access(fileLocation) == F_OK) {
        char *input = (char *) calloc(2, sizeof(char));

        app->plainText = fopen(fileLocation, "r+");
        fgets(input, 2, app->plainText);
        return input;
    } else {
        return NULL;
    }
}

char* fileToText(App *app) {
    // Max of 10,000 characters for now
    char *text = (char *) calloc(10000, sizeof(char));

    while (!feof(app->plainText)) {
        char input[BUFSIZ];
        fgets(input, BUFSIZ, app->plainText);
        strncat(text, input, BUFSIZ);
    }
    return text;
}

char* encryptText(char *plainText, App *app) {
    // typecasting char* to double = WARNING!!
    leaf_encrypt(app->encryptKey, plainText);
}

/**
Sets up the home path of the App
@return the home path of the app
*/
void setupHomePath(char *appName, App *app) {
    // Makes a directory with mkdir in LOCALAPPDATA
    app->homePath = (char *) calloc(PATHSIZE, sizeof(char));
    char *localDir = getenv("LOCALAPPDATA");
    snprintf(app->homePath, PATHSIZE, "%s\\%s", localDir, appName);

    // Make sure the path is created
    mkdir(app->homePath);

    // Free resources
    free(localDir);
}

/**
Sets up a log path and saves it to App
*/
void setupLogPath(App *app) {
    app->logPath = (char *) calloc(PATHSIZE, sizeof(char));
    char logName[] = "Log";
    snprintf(app->logPath, PATHSIZE, "%s\\%s.log", app->homePath, logName);
}

/**
Sets up the registry stuff
*/
void setupRegistry(App *app) {
    int status = isRegFileOK(app);

    if (status == false) {
        createRegAndLogIt();
    }
    /// TODO: if status == -1 or true, you need to handle it!
}



/// Helper Methods ///

static void createRegAndLogIt() {
    FILE *createIt = fopen(path, "w");
    char result = createRegistryEntries(CONTEXTMENUITEM, app);
    fprintf(createIt, "%c", result);

    // Free up the resources
    fclose(createIt);
}

static bool createRegistryEntries(char optionName[], App *app) {
    char createShell[] = "REG ADD HKCU\\SOFTWARE\\Classes\\.txt\\shell";

    char regWithOption[PATHSIZE];
    snprintf(regWithOption, PATHSIZE, "%s\\%s", createShell, optionName);

    char commandReg[PATHSIZE];
    snprintf(commandReg, PATHSIZE, "%s\\command", regWithOption);

    char defaultValue[PATHSIZE];
    snprintf(defaultValue, PATHSIZE, "%s /ve /d \"\"%s\" \"%1\"\" /f", commandReg, app->appLocation);

    // Execute all the commands
    /// TODO: Add /f later to the strings prevent confirmation dialog
    system(createShell);
    system(regWithOption);
    system(commandReg);
    system(defaultValue);

    return true;
}

// TODO: Change the name of this method
static int isRegFileOK(App *app) {
    app->regPath = (char *) calloc(PATHSIZE, sizeof(char));
    snprintf(app->regPath, PATHSIZE, "%s\\%s.txt", app->homePath, REGFILECHECK);

    if (access(path, "r") == F_OK) {
        FILE *opened = fopen(path, "r");
        char input = (char) fgetc(input);
        if (input == 'T') {
            // Is true == 1?
            return true;
        } else if (input == 'F') {
            // Log false in log file
            return -1;
        } else {
            // Log unknown error happened in log file
            return -1;
        }
    } else {
        return false;
    }

}

/**
Frees all the RAM held by the App data block.
*/
void freeApp(App *app) {
    // I don't know if this works, but I will use it anyway
    free(app->homePath);
    free(app->logPath);
    free(app);
}
