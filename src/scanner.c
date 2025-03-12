#include "tree_sitter/alloc.h"
#include "tree_sitter/array.h"
#include "tree_sitter/parser.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

enum TokenType { INDENT = 0, DEDENT, NEWLINE, ERROR_SENTINEL };

typedef struct {
  uint8_t indent_width;
  uint8_t last_indent_level;
} State;

void *tree_sitter_moonscript_external_scanner_create() {
  return ts_calloc(1, sizeof(State));
}

void tree_sitter_moonscript_external_scanner_destroy(State *state) {
  ts_free(state);
}

unsigned tree_sitter_moonscript_external_scanner_serialize(State *state,
                                                           char *buffer) {
  size_t count = sizeof(State);
  assert(count < TREE_SITTER_SERIALIZATION_BUFFER_SIZE);
  memcpy(buffer, state, count);
  return count;
}

void tree_sitter_moonscript_external_scanner_deserialize(State *state,
                                                         const char *buffer,
                                                         unsigned length) {
  State parsed_state = {0};
  memcpy(&parsed_state, buffer, length);
  state->indent_width = parsed_state.indent_width;
  state->last_indent_level = parsed_state.last_indent_level;
}

bool tree_sitter_moonscript_external_scanner_scan(State *state, TSLexer *lexer,
                                                  const bool *valid_symbols) {
  if (valid_symbols[ERROR_SENTINEL] || lexer->lookahead != '\n') {
    return false;
  }
  lexer->advance(lexer, true);

  while (!lexer->eof(lexer)) {
    switch (lexer->lookahead) {
    case ' ':
    case '\t': {
      if (state->indent_width == 0) {
        while (lexer->lookahead == ' ' || lexer->lookahead == '\t') {
          if (lexer->lookahead == ' ')
            state->indent_width += 1;

          if (lexer->lookahead == '\t')
            state->indent_width += 4;

          lexer->advance(lexer, true);
        }
        assert(state->indent_width > 0);
        state->last_indent_level = 1;
        lexer->result_symbol = INDENT;
        return true;
      } else {
        size_t current_indent_width = 0;
        while (lexer->lookahead == ' ' || lexer->lookahead == '\t') {
          if (lexer->lookahead == ' ')
            current_indent_width += 1;

          if (lexer->lookahead == '\t')
            current_indent_width += 4;

          lexer->advance(lexer, true);
        }
        size_t current_indent_level =
            current_indent_width / state->indent_width;

        if (state->last_indent_level < current_indent_level) {
          state->last_indent_level = current_indent_level;
          lexer->result_symbol = INDENT;
          return true;
        } else if (current_indent_level < state->last_indent_level) {
          state->last_indent_level = current_indent_level;
          lexer->result_symbol = DEDENT;
          return true;
        }
        return false;
      }
      break;
    }
    default: {
      if (state->last_indent_level > 0) {
        state->last_indent_level = 0;
        lexer->result_symbol = DEDENT;
        return true;
      }
      return false;
    }
    }
  }
  lexer->result_symbol = DEDENT;
  return true;
}
