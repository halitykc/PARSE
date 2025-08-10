#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../libft/libft.h"

typedef struct {
    char **tokens;
    int size;
    int capacity;
} TokenArray;

void add_token(TokenArray *arr, char *token)
{
    if (arr->size == arr->capacity)
    {
        int i; 
        int new_cap;
        char **new_tokens;

        new_cap = arr->capacity * 2;
        new_tokens = malloc(new_cap * sizeof(char *));
        i = -1;
        while (++i < arr->size)
            new_tokens[i] = arr->tokens[i];
        free(arr->tokens);
        arr->tokens = new_tokens;
        arr->capacity = new_cap;
    }
    arr->tokens[arr->size++] = token;
}

// Tırnakları göz önünde bulundurarak bölme 
char **quote_aware_split(const char *input)
{
    TokenArray arr;
    arr.size = 0;
    arr.capacity = 10;
    arr.tokens = malloc(arr.capacity * sizeof(char *));

    int i = 0;
    int len = ft_strlen(input);
    while (i < len)
    {
        while (i < len && isspace(input[i]))
            i++; // boşlukları atla

        if (i >= len)
            break;

        char *token = NULL;
        int start = i;

        if (input[i] == '\'' || input[i] == '"')
        {
            char quote_char = input[i++];
            start = i;
            while (i < len && input[i] != quote_char)
                i++;

            int tok_len = i - start;
            token = malloc(tok_len + 1);
            strncpy(token, input + start, tok_len);
            token[tok_len] = '\0';

            if (i < len)
                i++; // kapama tırnağını atla

            // Tırnakları korumak için başına ve sonuna ekleyelim
            char *with_quotes = malloc(tok_len + 3);
            with_quotes[0] = quote_char;
            ft_memcpy(with_quotes + 1, token, tok_len);
            with_quotes[tok_len + 1] = quote_char;
            with_quotes[tok_len + 2] = '\0';

            free(token);
            token = with_quotes;
        }
        else
        {
            start = i;
            while (i < len && !isspace(input[i]) && input[i] != '\'' && input[i] != '"')
                i++;

            int tok_len = i - start;
            token = malloc(tok_len + 1);
            strncpy(token, input + start, tok_len);
            token[tok_len] = '\0';
        }

        add_token(&arr, token);
    }

    add_token(&arr, NULL); // sona NULL ekle
    return arr.tokens;
}

// Tırnakları boşluk varsa koruyan, yoksa kaldıran fonksiyon
char *process_token(char *token)
{
    int len = ft_strlen(token);
    if (len < 2)
        return ft_strdup(token);

    char first = token[0];
    char last = token[len - 1];

    if ((first == '\'' && last == '\'') || (first == '"' && last == '"'))
    {
        int has_space = 0;
        int i = 0;
        while (++i < len - 1)
        {
            if (token[i] == ' ')
            {
                has_space = 1;
                break;
            }
        }

        if (has_space)
        {
            // Burada özel durum kontrolü yap:
            // Eğer dış tırnak " ve içinde ' varsa (örneğin "'ls -l'")
            // dıştaki " kaldırılıp içtekiler kalacak
            if (first == '"' && ft_strchr(token + 1, '\''))
            {
                // Dıştaki çift tırnağı kaldır
                char *res = malloc(len - 1);
                if (!res) return NULL;
                ft_memcpy(res, token + 1, len - 2);
                res[len - 2] = '\0';
                return res;
            }
            // Aynı şekilde tersi durumda:
            if (first == '\'' && ft_strchr(token + 1, '"'))
            {
                char *res = malloc(len - 1);
                if (!res) return NULL;
                ft_memcpy(res, token + 1, len - 2);
                res[len - 2] = '\0';
                return res;
            }

            // Normalde dıştaki tırnaklar kalacak
            return ft_strdup(token);
        }
        else
        {
            char *res = malloc(len - 1);
            ft_memcpy(res, token + 1, len - 2);
            res[len - 2] = '\0';
            return res;
        }
    }
    return ft_strdup(token);
}


int main()
{
    char *input = "< file1 'ls' '-l' | \"'wc'\" \"'-l'\" > file2";

    char **tokens = quote_aware_split(input);

    for (int i = 0; tokens[i] != NULL; i++)
    {
        char *proc = process_token(tokens[i]);
        printf("Orjinal: %s : TOKEN [%i]: %s\n", tokens[i], i, proc);
        free(proc);
        free(tokens[i]);
    }
    free(tokens);

    return 0;
}
