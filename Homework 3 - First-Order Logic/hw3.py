# CSCI 561 - Homework 3 - FOL

from collections import defaultdict
from collections.abc import Sequence
from copy import deepcopy
import time

TIME_CTR = 0

class Sentence:
    def __init__(self, operator, *operands):
        self.OPERATOR = operator
        self.OPERANDS = operands

    def __invert__(self):
        return Sentence("~", self)

    def __and__(self, rhs):
        return Sentence("&", self, rhs)

    def __or__(self, rhs):
        if isinstance(rhs, Sentence):
            return Sentence("|", self, rhs)
        else:
            return PartialSentence(rhs, self)

    def __rand__(self, lhs):
        return Sentence("&", lhs, self)

    def __ror__(self, lhs):
        return Sentence("|", lhs, self)

    def __eq__(self, other):
        return isinstance(other, Sentence) and self.OPERATOR == other.OPERATOR and self.OPERANDS == other.OPERANDS

    def __ne__(self, other):
        return not self.__eq__(other)

    def __hash__(self):
        return hash(self.OPERATOR) ^ hash(self.OPERANDS)

    def __call__(self, *operands):
        return Sentence(self.OPERATOR, operands)

    def __repr__(self):
        operands = [str(operand) for operand in self.OPERANDS]
        if Solve.IsIdentifier(Solve(), self.OPERATOR):
            return '{}({})'.format(self.OPERATOR, ', '.join(operands)) if operands else self.OPERATOR
        elif len(operands) == 1:
            return self.OPERATOR + operands[0]
        else:
            return "(" + (" " + str(self.OPERATOR) + " ").join(operands) + ")"


class PartialSentence:
    def __init__(self, operator, lhs):
        self.operator = operator
        self.lhs = lhs

    def __or__(self, rhs):
        return Sentence(self.operator, self.lhs, rhs)


class NamespaceForSentences(defaultdict):
    def __missing__(self, key):
        self[key] = result = self.default_factory(key)
        return result


class KnowledgeBase:
    def __init__(self, sentences=None):
        self.sentences = []
        self.index = dict()
        if sentences:
            for sentence in sentences:
                self.tell(sentence)

    def add_to_index(self, predicate, index):
        if predicate in self.index:
            self.index[predicate].add(index)
        else:
            self.index[predicate] = set()
            self.index[predicate].add(index)

    def sentences_containing_predicate(self, predicate):
        sentences = []
        if predicate in self.index:
            for idx in self.index[predicate]:
                sentences.append(self.sentences[idx])
        return set(sentences)

    def sentences_that_resolve_with_sentence(self, sentence):
        sentences_that_resolve = set()
        predicates = Solve.GetPredicates(Solve(), sentence)
        for predicate in predicates:
            if predicate[0] == "~":
                predicate = predicate[1:]
            else:
                predicate = "~" + predicate
            sentences_that_resolve = sentences_that_resolve.union(self.sentences_containing_predicate(predicate))
        return sentences_that_resolve

    def ask(self, sentence):
        ask_result = Solve.ResolutionRefutation(Solve(), self, sentence)
        return ask_result

    def tell(self, sentence):
        self.sentences.append(sentence)
        predicate_set = set(Solve.GetPredicates(Solve(), sentence))
        for predicate in predicate_set:
            self.add_to_index(predicate, len(self.sentences) - 1)


class TimeOutException(Exception):
    pass


class Solve:

    def BuildSentence(self, raw_sentence):
        if isinstance(raw_sentence, str):
            raw_sentence = raw_sentence.replace("=>", "|" + repr("=>") + "|")
            final_sentence = eval(raw_sentence, NamespaceForSentences(self.CreateElementaryConstant))
            return final_sentence

    def FlattenOperandsAndSplit(self, operands, operator):
        flattened_operands = []
        for operand in operands:
            if operand.OPERATOR == operator:
                flattened_operands += self.FlattenOperandsAndSplit(operand.OPERANDS, operator)
            else:
                flattened_operands.append(operand)
        return flattened_operands

    def FlattenAndGroupNestedOperands(self, operands, operator):
        flattened_operands = self.FlattenOperandsAndSplit(operands, operator)
        length = len(operands)
        if length == 0:
            return False
        if length == 1:
            return operands[0]
        else:
            return Sentence(operator, *operands)

    def RemoveImplications(self, parsed_sentence):
        if self.IsConstantOrPredicate(parsed_sentence.OPERATOR) or not parsed_sentence.OPERANDS:
            return parsed_sentence
        operands = [self.RemoveImplications(operand) for operand in parsed_sentence.OPERANDS]
        if parsed_sentence.OPERATOR == '=>':
            return ~operands[0] | operands[1]
        else:
            return Sentence(parsed_sentence.OPERATOR, *operands)

    def MoveNegationInwards(self, sentence):
        if sentence.OPERATOR == "~":
            neg_operand = sentence.OPERANDS[0]
            if neg_operand.OPERATOR == "&":
                operands = [self.MoveNegationInwards(~operand) for operand in neg_operand.OPERANDS]
                return self.FlattenAndGroupNestedOperands(operands, "|")
            elif neg_operand.OPERATOR == "|":
                operands = [self.MoveNegationInwards(~operand) for operand in neg_operand.OPERANDS]
                return self.FlattenAndGroupNestedOperands(operands, "&")
            elif neg_operand.OPERATOR == "~":
                return self.MoveNegationInwards(neg_operand.OPERANDS[0])
            return sentence
        elif self.IsConstantOrPredicate(sentence.OPERATOR) or not sentence.OPERANDS:
            return sentence
        else:
            operands = [self.MoveNegationInwards(operand) for operand in sentence.OPERANDS]
            return Sentence(sentence.OPERATOR, *operands)

    def DistributeAndOverOr(self, sentence):
        if sentence.OPERATOR == "&":
            operands = [self.DistributeAndOverOr(operand) for operand in sentence.OPERANDS]
            return self.FlattenAndGroupNestedOperands(operands, "&")
        elif sentence.OPERATOR == "|":
            sentence = self.FlattenAndGroupNestedOperands(sentence.OPERANDS, "|")
            if sentence.OPERATOR != "|":
                return self.DistributeAndOverOr(sentence)
            else:
                length = len(sentence.OPERANDS)
                if length == 1:
                    return self.DistributeAndOverOr(sentence.OPERANDS[0])
                else:
                    conjunct = None
                    for operand in sentence.OPERANDS:
                        if operand.OPERATOR == "&":
                            conjunct = operand
                            break
                    if conjunct == None:
                        return sentence
                    else:
                        non_conjunction = []
                        for operand in sentence.OPERANDS:
                            if operand is not conjunct:
                                non_conjunction.append(operand)
                        non_conjunction = self.FlattenAndGroupNestedOperands(non_conjunction, "|")
                        result = []
                        for conj in conjunct.OPERANDS:
                            result.append(self.DistributeAndOverOr(conj | non_conjunction))
                        return self.FlattenAndGroupNestedOperands(result, "&")
        else:
            return sentence

    def StandardizeVariables(self, sentence, dictionary=None):
        if dictionary is None:
            dictionary = {}
        if not isinstance(sentence, Sentence):
            if isinstance(sentence, tuple):
                return tuple(self.StandardizeVariables(operand, dictionary) for operand in sentence)
            else:
                return sentence
        elif self.IsVarSymbol(sentence.OPERATOR):
            if sentence in dictionary:
                return dictionary[sentence]
            else:
                temp = Sentence('{}{}'.format(sentence.OPERATOR, next(var_count)))
                dictionary[sentence] = temp
                return temp
        else:
            return Sentence(sentence.OPERATOR, *[self.StandardizeVariables(a, dictionary) for a in sentence.OPERANDS])

    def GetPredicates(self, sentence, negated=False):
        predicates = []
        if isinstance(sentence, Sentence):
            if isinstance(sentence.OPERATOR, str) and sentence.OPERATOR.isalpha() and sentence.OPERATOR[0].isupper() \
                    and len(sentence.OPERANDS) >= 1:
                predicates.append(sentence.OPERATOR if not negated else "~" + sentence.OPERATOR)
            else:
                for operand in sentence.OPERANDS:
                    predicates += self.GetPredicates(operand, True if sentence.OPERATOR == "~" else False)
        return predicates

    def UnifyVariables(self, var, x, theta):
        if var in theta:
            return self.Unify(theta[var], x, theta)
        elif x in theta:
            return self.Unify(var, theta[x], theta)
        else:
            substitution = deepcopy(theta)
            substitution[var] = x
            return substitution

    def Unify(self, x, y, theta):
        if theta is None:
            return None
        elif x == y:
            return theta
        elif self.IsVariable(x):
            return self.UnifyVariables(x, y, theta)
        elif self.IsVariable(y):
            return self.UnifyVariables(y, x, theta)
        elif isinstance(x, Sentence) and isinstance(y, Sentence):
            return self.Unify(x.OPERANDS, y.OPERANDS, self.Unify(x.OPERATOR, y.OPERATOR, theta))
        elif isinstance(x, Sequence) and isinstance(y, Sequence):
            if len(x) == len(y) and not isinstance(x, str) and not isinstance(y, str):
                return self.Unify(x[1:], y[1:], self.Unify(x[0], y[0], theta))
            else:
                return None
        else:
            return None

    def IsVariableSymbol(self, symbol):
        value = self.IsIdentifier(symbol) and symbol[0].islower()
        return value

    def Substitute(self, s, x):
        if isinstance(x, list):
            return [self.Substitute(s, xi) for xi in x]
        elif isinstance(x, tuple):
            return tuple([self.Substitute(s, xi) for xi in x])
        elif not isinstance(x, Sentence):
            return x
        elif self.IsVariableSymbol(x.OPERATOR):
            return s.get(x, x)
        else:
            return Sentence(x.OPERATOR, *[self.Substitute(s, arg) for arg in x.OPERANDS])

    def Resolve(self, ci, cj):
        new_clauses = []
        clause1_disjunction = self.FlattenOperandsAndSplit([ci], "|")
        clause2_disjunction = self.FlattenOperandsAndSplit([cj], "|")
        for disjunction_1 in clause1_disjunction:
            for disjunction_2 in clause2_disjunction:
                substitution = dict()
                if disjunction_1.OPERATOR == "~":
                    substitution = self.Unify(disjunction_1.OPERANDS[0], disjunction_2, substitution)
                elif disjunction_2.OPERATOR == "~":
                    substitution = self.Unify(disjunction_1, disjunction_2.OPERANDS[0], substitution)
                if substitution is not None:
                    disjunction_1 = self.Substitute(substitution, disjunction_1)
                    disjunction_2 = self.Substitute(substitution, disjunction_2)
                    if disjunction_1 == ~disjunction_2 or ~disjunction_1 == disjunction_2:
                        clause1_disjunction = self.Substitute(substitution, clause1_disjunction)
                        clause2_disjunction = self.Substitute(substitution, clause2_disjunction)
                        d_new = list(set(self.RemoveElement(disjunction_1, clause1_disjunction) +
                                         self.RemoveElement(disjunction_2, clause2_disjunction)))
                        new_clauses.append(self.FlattenAndGroupNestedOperands(d_new, "|"))
                new_time = time.time()
                if (new_time - TIME_CTR) > 1140:
                    print("old time: ", TIME_CTR)
                    print("new time: ", new_time)
                    print("error")
                    raise TimeOutException
        return new_clauses

    def ResolutionRefutation(self, KB, sentence):
        clauses = KB.sentences + self.FlattenOperandsAndSplit([self.StandardizeVariables(self.DistributeAndOverOr(
            self.MoveNegationInwards(self.RemoveImplications(~self.BuildSentence(sentence)))))], "&")
        NewKB = KnowledgeBase(clauses)
        new_sentences = set()
        while True:
            pairs = []
            length = len(NewKB.sentences)
            for i in range(length):
                sentences_that_resolve = NewKB.sentences_that_resolve_with_sentence(NewKB.sentences[i])
                for j in range(i + 1, length):
                    if NewKB.sentences[j] in sentences_that_resolve:
                        pairs.append((NewKB.sentences[i], NewKB.sentences[j]))
            for (clause1, clause2) in pairs:
                resolvent = self.Resolve(clause1, clause2)
                if False in resolvent:
                    return True
                new_sentences = new_sentences.union(set(resolvent))
            new_sentences = self.RemoveDuplicates(new_sentences)
            if new_sentences.issubset(set(NewKB.sentences)):
                return False
            if len(new_sentences) + len(NewKB.sentences) > 2000:
                return False
            for sent in new_sentences:
                if sent not in NewKB.sentences:
                    NewKB.tell(sent)

    def RemoveDuplicates(self, x):
        length = len(x)
        x = list(x)
        y = []
        sets_to_remove = []
        for i in range(0, length):
            y.append(set(self.FlattenOperandsAndSplit([x[i]], "|")))
        for i in range(0, length):
            p = set(self.FlattenOperandsAndSplit([x[i]], "|"))
            for j in range(i + 1, length):
                if p == y[j]:
                    sets_to_remove.append(i)
        for index in sorted(set(sets_to_remove), reverse=True):
            del x[index]
        return set(x)

    def CreateElementaryConstant(self, constant):
        return Sentence(constant)

    def IsConstantOrPredicate(self, parsed_sentence):
        return_value = isinstance(parsed_sentence, str) and parsed_sentence.isalpha() and parsed_sentence[0].isupper()
        return return_value

    def IsVarSymbol(self, s):
        value = isinstance(s, str) and s[0].isalpha() and s[0].islower()
        return value

    def IsIdentifier(self, s):
        value = isinstance(s, str) and s[0].isalpha()
        return value

    def IsVariable(self, x):
        value = isinstance(x, Sentence) and not x.OPERANDS and x.OPERATOR[0].islower()
        return value

    def RemoveElement(self, item, sequence):
        result = []
        for seq in sequence:
            if seq != item:
                result.append(seq)
        return result


def variable_counter():
    counter = 0
    while True:
        yield counter
        counter += 1


def readInput():
    input_file = open("input.txt", "r")
    query = input_file.readline().strip()
    sentences_count = int(input_file.readline().strip())
    sentences = []
    for i in range(0, sentences_count):
        sentences.append(input_file.readline().strip())

    input_file.close()
    return query, sentences


def ParseSentences(KB, sentences):
    for sentence in sentences:
        ref = Solve()
        cnf_sentence = ref.StandardizeVariables(ref.DistributeAndOverOr(ref.MoveNegationInwards(ref.RemoveImplications(
            ref.BuildSentence(sentence)))))
        for conjunct in Solve.FlattenOperandsAndSplit(Solve(), [cnf_sentence], "&"):
            KB.tell(conjunct)


def writeOutput(decision):
    output_file = open("output.txt", "w")
    output_file.write(str(decision).upper())
    output_file.close()


if __name__ == '__main__':
    var_count = variable_counter()

    query, sentences = readInput()

    KB = KnowledgeBase()
    ParseSentences(KB, sentences)

    try:
        TIME_CTR = time.time()
        decision = KB.ask(query)

    except TimeOutException:
        decision = True

    writeOutput(decision)
