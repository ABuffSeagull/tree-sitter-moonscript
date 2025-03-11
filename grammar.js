/**
 * @file Moonscript grammar for tree-sitter
 * @author ABuffSeagull <reecevanatta@hey.com>
 * @license MIT
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
  name: "moonscript",

  rules: {
    // TODO: add the actual grammar rules
    source_file: $ => "hello"
  }
});
