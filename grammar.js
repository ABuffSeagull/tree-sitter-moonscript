/**
 * @file Moonscript grammar for tree-sitter
 * @author ABuffSeagull <reecevanatta@hey.com>
 * @license MIT
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

const multi = (inner) => seq(inner, repeat(seq(",", inner)));

module.exports = grammar({
	name: "moonscript",

	externals: ($) => [$.indent, $.dedent, $.error_sentinel],
	extras: ($) => [$.comment, new RustRegex("\\s")],

	reserved: {
		global: ($) => ["export", "local"],
	},

	rules: {
		source_file: ($) => repeat($._statement),

		_statement: ($) =>
			seq(choice($.assignment, multi($._expression)), choice("\n", "\0")),

		comment: ($) => new RustRegex("--.*"),

		_expression: ($) =>
			prec(
				0,
				choice(
					$.bin_op,
					$.unary_op,
					$.boolean,
					$.string,
					$.number,
					$.nil,
					$.identifier,
					seq("(", $._expression, ")"),
				),
			),

		assignment: ($) =>
			prec(
				1,
				seq(
					optional(choice("export", "local")),
					field("destination", multi($.identifier)),
					$._assignment_op,
					field("source", multi($._expression)),
				),
			),

		_assignment_op: ($) =>
			choice("=", "+=", "-=", "/=", "*=", "%=", "..=", "and=", "or="),

		bin_op: ($) =>
			prec.right(
				0,
				seq(
					field("left", $._expression),
					$._bin_operator,
					field("right", $._expression),
				),
			),

		_bin_operator: ($) =>
			choice("+", "-", "*", "/", "and", "or", "..", "==", "!="),

		unary_op: ($) => prec(1, seq("-", $._expression)),

		identifier: ($) => new RustRegex("[a-zA-Z]+"),
		number: ($) => new RustRegex("\\d+"),
		string: ($) => seq('"', repeat(new RustRegex(".")), '"'),
		boolean: ($) => choice("true", "false"),
		nil: ($) => "nil",
	},
});
