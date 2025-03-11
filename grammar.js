/**
 * @file Moonscript grammar for tree-sitter
 * @author ABuffSeagull <reecevanatta@hey.com>
 * @license MIT
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
  name: "moonscript",

  externals: ($) => [$.indent, $.dedent],
  extras: ($) => ["\n"],

  rules: {
    source_file: ($) => repeat(seq(optional($.indent), $.identitier)),
    identitier: ($) => new RustRegex("\\w+"),
  },
});
