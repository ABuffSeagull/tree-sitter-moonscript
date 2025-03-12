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

	reserved: {
		global: ($) => ["export", "local"],
	},

	rules: {
		source_file: ($) => repeat($.assignment),

		_expression: ($) => choice($.boolean, $.string, $.number, $.identifier),

		assignment: ($) =>
			seq(
				optional(choice("export", "local")),
				field("destination", $.identifier),
				$._assignment_op,
				field("source", $._expression),
			),

		_assignment_op: ($) =>
			choice("=", "+=", "-=", "/=", "*=", "%=", "..=", "and=", "or="),

		identifier: ($) => new RustRegex("[a-zA-Z]+"),
		number: ($) => new RustRegex("\\d+"),
		string: ($) => new RustRegex('".*"'),
		boolean: ($) => choice("true", "false"),
	},
});
