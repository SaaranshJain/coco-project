# NOTE: This file is not part of the final compiler. It is used to parse the grammar rules written in a simple format and convert them into a C header file.
HEADER_FILE = "grammar.h"
SOURCE_GRAMMAR_FILE = "grammar.txt"

def parseGrammar():
    with open(SOURCE_GRAMMAR_FILE, "r") as f:
        lines = f.readlines()
    
    with open(HEADER_FILE, "w") as f:
        f.write("""#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <stdlib.h>
#include <string.h>
#include "parserDef.h"

Grammar getLanguageGrammar(){\n""")
        seen_symbols = set()
        rule_num = 0
        for line in lines:
            rule = line.split("-->")
            if len(rule) != 2:
                print("Invalid rule: ", line)
                continue
            lhs = rule[0].strip()
            rhs_arr = rule[1].strip().split("|")
            for rhs in rhs_arr:
                rhs = rhs.strip().split(" ")
                rule_rhs = []
                for rhs_elem in rhs:
                    is_epsilon = rhs_elem == "eps"
                    is_terminal = not (rhs_elem[0] == "<" and rhs_elem[-1] == ">")

                    if is_terminal:
                        rule_rhs.append(rhs_elem.lower())
                        if rhs_elem in seen_symbols:
                            continue
                        f.write("\tRuleRightSideElement " + rhs_elem.lower() + " = (RuleRightSideElement) malloc(sizeof(struct ruleRightSideElement));\n")
                        f.write("\t*" + rhs_elem.lower() + " = (struct ruleRightSideElement) {")
                        f.write(str(is_epsilon).lower() + ", " + str(is_terminal).lower() + ", ")
                        f.write(f"{{.terminal = {'TK_ID' if is_epsilon else rhs_elem} }}")
                        f.write("};\n")

                    else:
                        rule_rhs.append(rhs_elem[1:-1])
                        if rhs_elem in seen_symbols:
                            continue
                        f.write("\tRuleRightSideElement " + rhs_elem[1:-1] + " = (RuleRightSideElement) malloc(sizeof(struct ruleRightSideElement));\n")
                        f.write("\t*" + rhs_elem[1:-1] + " = (struct ruleRightSideElement) {")
                        f.write(str(is_epsilon).lower() + ", " + str(is_terminal).lower() + ", ")
                        f.write("{.nonTerminal = NT_" + rhs_elem[1].capitalize() + rhs_elem[2:-1] + "}")
                        f.write("};\n")
                    seen_symbols.add(rhs_elem)

                f.write ("\tRuleRightSideElement *rhs" + str(rule_num) + " = (RuleRightSideElement *) malloc(sizeof(RuleRightSideElement) * " + str(len(rule_rhs)) + ");\n")
                f.write("\tmemcpy(rhs" + str(rule_num) + ", (RuleRightSideElement[]) {")
                for rhs_elem in rule_rhs:
                    f.write(f"{rhs_elem}, ")
                f.write("}, sizeof(struct ruleRightSideElement) * " + str(len(rule_rhs)) +");\n")
                f.write("\tRule rule" + str(rule_num) + " = (Rule) malloc(sizeof(struct rule));\n")
                f.write("\t*rule" + str(rule_num) + " = (struct rule) {")
                f.write(".lhs = NT_" + lhs[1].capitalize() + lhs.strip()[2:-1] + ", ")
                f.write(".rhs = rhs" + str(rule_num) + ", ")
                f.write(".rhsLength = " + str(len(rule_rhs)) + "};\n")
                rule_num += 1
        f.write("\n\tRule *rules = (Rule *) malloc(sizeof(Rule) * " + str(rule_num) + ");\n")
        f.write("\tmemcpy(rules, (Rule[]) {")
        for i in range(rule_num):
            f.write("rule" + str(i) + ", ")
        f.write("}, " + str(rule_num) + " * sizeof(Rule));\n")
        f.write("\tGrammar languageGrammar = (Grammar) malloc(sizeof(struct grammar));\n")
        f.write("\t*languageGrammar = (struct grammar) {NT_Program, " + str(rule_num) + ", rules};\n")
        f.write("\treturn languageGrammar;\n")
        f.write("}\n")

        f.write("""
void freeGrammar(Grammar g) {
    if (g == NULL) return;

    // Array to track freed elements
    RuleRightSideElement *freedElements = (RuleRightSideElement *) malloc(sizeof(RuleRightSideElement) * (g->numRules * 10));
    int freedCount = 0;

    // Free each rule and its right-hand side elements
    for (int i = 0; i < g->numRules; i++) {
        Rule rule = g->rules[i];

        for (int j = 0; j < rule->rhsLength; j++) {
            RuleRightSideElement elem = rule->rhs[j];

            // Check if the element was already freed
            int alreadyFreed = 0;
            for (int k = 0; k < freedCount; k++) {
                if (freedElements[k] == elem) {
                    alreadyFreed = 1;
                    break;
                }
            }

            // Free only if not already freed
            if (!alreadyFreed) {
                freedElements[freedCount++] = elem;
                free(elem);
            }
        }

        free(rule->rhs);
        free(rule);
    }

    free(freedElements);
    free(g->rules);
    free(g);
}\n""")

        f.write(f"""
#define NUM_NON_TERMINALS {len(seen_symbols)}
#define NUM_RULES {str(rule_num)}
#endif""")

if __name__ == "__main__":
    parseGrammar()