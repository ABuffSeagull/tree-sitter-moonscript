/**
 * @file Moonscript grammar for tree-sitter
 * @author ABuffSeagull <reecevanatta@hey.com>
 * @license MIT
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
	name: "moonscript",

	externals: ($) => [$.indent, $.dedent, $.error_sentinel],
	// extras: ($) => ["\n"],

	rules: {
		source_file: ($) => repeat($._line),
		_line: ($) => choice($.identifier, seq($.indent, $.identifier), $.dedent),
		identifier: ($) => new RustRegex("\\d+"),
	},
});
