# NOTE: This file is not part of the final compiler. It is used to parse the grammar rules written in a simple format and convert them into a C header file.
HEADER_FILE = "grammar.h"
SOURCE_GRAMMAR_FILE = "grammar.txt"

def parseGrammar():
    with open(SOURCE_GRAMMAR_FILE, "r") as f:
        lines = f.readlines()
    
    with open(HEADER_FILE, "w") as f:
        f.write("""#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "parserDef.h"\n""")
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
                        f.write("struct ruleRightSideElement _" + rhs_elem.lower() + " = {")
                        f.write(str(is_epsilon).lower() + ", " + str(is_terminal).lower() + ", ")
                        f.write(f".symbol.terminal = {'TK_ID' if is_epsilon else rhs_elem}")
                        f.write("};\n")

                    else:
                        rule_rhs.append(rhs_elem[1:-1])
                        if rhs_elem in seen_symbols:
                            continue
                        f.write("struct ruleRightSideElement _" + rhs_elem[1:-1] + " = {")
                        f.write(str(is_epsilon).lower() + ", " + str(is_terminal).lower() + ", ")
                        f.write(".symbol.nonTerminal = NT_" + rhs_elem[1].capitalize() + rhs_elem[2:-1])
                        f.write("};\n")
                    seen_symbols.add(rhs_elem)

                f.write ("RuleRightSideElement _rhs" + str(rule_num) + "[] = {")
                for rhs_elem in rule_rhs:
                    f.write(f"&_{rhs_elem}, ")
                f.write("};\n")
                f.write("struct rule _rule" + str(rule_num) + " = {")
                f.write(".lhs = NT_" + lhs[1].capitalize() + lhs.strip()[2:-1] + ", ")
                f.write(".rhs = _rhs" + str(rule_num) + ", ")
                f.write(".rhsLength = " + str(len(rule_rhs)) + "};\n")
                rule_num += 1
        f.write("\nRule _grammarRules[] = {")
        for i in range(rule_num):
            f.write("&_rule" + str(i) + ", ")
        f.write("};\n")
        f.write("struct grammar languageGrammar = {NT_Program, " + str(rule_num) + ", _grammarRules};\n")
        f.write("#define NUM_NON_TERMINALS " + str(len(seen_symbols)) + "\n")
        f.write("#define NUM_RULES " + str(rule_num) + "\n")
        f.write("#endif\n")

if __name__ == "__main__":
    parseGrammar()