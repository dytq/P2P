#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_csv_value(const char* csv_line, int line_number) {
    char* line_copy = strdup(csv_line); // Dupliquer la ligne pour éviter de modifier la chaîne d'origine
    char* token = strtok(line_copy, ",");
    int current_line = 1;

    while (token != NULL) {
        if (current_line == line_number) {
            // Nous avons trouvé la valeur à la ligne spécifiée

            // Vérifier si le dernier caractère est un '\n' et le remplacer par '\0'
            size_t length = strlen(token);
            if (length > 0 && token[length - 1] == '\n') {
                token[length - 1] = '\0';
            }

            char* result = strdup(token);
            free(line_copy); // Libérer la mémoire allouée pour la copie de ligne
            return result;
        }

        token = strtok(NULL, ",");
        current_line++;
    }

    free(line_copy); // Libérer la mémoire allouée pour la copie de ligne
    return NULL; // La ligne spécifiée n'a pas été trouvée
}

