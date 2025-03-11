#include "tree_sitter/parser.h"
#include "tree_sitter/alloc.h"
#include "tree_sitter/array.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

enum TokenType {
    INDENT = 0,
    DEDENT
};

typedef struct {
    uint8_t indent_width;
    uint8_t last_indent_level;
} State;

void * tree_sitter_moonscript_external_scanner_create() {
    return ts_calloc(1, sizeof(State));
}

void tree_sitter_moonscript_external_scanner_destroy(State *payload) {
    ts_free(payload);
}

unsigned tree_sitter_moonscript_external_scanner_serialize(State *payload, char *buffer) {
    size_t count = sizeof(State);
    assert(count < TREE_SITTER_SERIALIZATION_BUFFER_SIZE);
    memcpy(buffer, payload, count);
    return count;
}

void tree_sitter_moonscript_external_scanner_deserialize(State *payload, const char *buffer, unsigned length) {
    State parsed_state = {0};
    memcpy(&parsed_state, buffer, length);
    payload->indent_width = parsed_state.indent_width;
    payload->last_indent_level = parsed_state.last_indent_level;
}

bool tree_sitter_moonscript_external_scanner_scan(State *state, TSLexer *lexer, const bool *valid_symbols) {
    printf("[Outer] Column: %d, Char: %d\n", lexer->get_column(lexer), lexer->lookahead);
    if (lexer->get_column(lexer) > 0) return false;

    while (!lexer->eof(lexer)) {
        printf("[Inner] Column: %d, Char: %d\n", lexer->get_column(lexer), lexer->lookahead);
        switch (lexer->lookahead) {
        case '\n': {
            lexer->advance(lexer, true);
            printf("Advanced...\n");
            break;
        }
        case ' ':
        case '\t': {
            if (state->indent_width == 0) {
                while (lexer->lookahead == ' ' || lexer->lookahead == '\t') {
                    if (lexer->lookahead == ' ') state->indent_width += 1;
                    if (lexer->lookahead == '\t') state->indent_width += 4;
                    lexer->advance(lexer, false);
                }
                state->indent_width = lexer->get_column(lexer);
                if (state->indent_width > 0) {
                    lexer->result_symbol = INDENT;
                    return true;
                }
                return false;
            } else {
                size_t current_indent_width = 0;
                while (lexer->lookahead == ' ' || lexer->lookahead == '\t') {
                    if (lexer->lookahead == ' ') current_indent_width += 1;
                    if (lexer->lookahead == '\t') current_indent_width += 4;
                    lexer->advance(lexer, false);
                }
                size_t current_indent_level = current_indent_width / state->indent_width;
                printf("last/current indent: %d/%ld\n", state->last_indent_level, current_indent_level);
                return false;
            }
            break;
        }
        default: {
            return false;
        }
        }
    }
    return false;
}
