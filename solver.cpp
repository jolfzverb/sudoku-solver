#include <iostream>
#include <set>
#include <vector>

#include "state.hpp"

enum class Command {
  kReadState,
  kPrintState,
  kRemovePencilMark,
  kFullPrint,
  kFill,
  kInsert,
  kIter,
  kReduce,
  kReducePairs,
  kExit,
  kUnknown,
};

Command GetNextCommand() {
  std::string command;
  std::getline(std::cin, command);

  if (std::cin.eof()) {
    return Command::kExit;
  }
  if (command == "r") {
    return Command::kReadState;
  }
  if (command == "rm") {
    return Command::kRemovePencilMark;
  }
  if (command == "p") {
    return Command::kPrintState;
  }
  if (command == "pp") {
    return Command::kFullPrint;
  }
  if (command == "q") {
    return Command::kExit;
  }
  if (command == "i" || command.empty()) {
    return Command::kIter;
  }
  if (command == "pmfill") {
    return Command::kFill;
  }
  if (command == "pminsert") {
    return Command::kInsert;
  }
  if (command == "pmreduce") {
    return Command::kReduce;
  }
  if (command == "pmreducepairs") {
    return Command::kReducePairs;
  }
  return Command::kUnknown;
}

int main() {
  Command command = Command::kReadState;
  state::State state;
  state.current_iter = 0;

  while (true) {
    state.current_iter++;
    switch (command) {
      case Command::kReadState:
        state = state::ReadState();
        state::FillPencilMarks(state);
        break;
      case Command::kRemovePencilMark:
        state::RemovePencilMark(state);
        break;
      case Command::kPrintState:
        state::PrintState(state);
        break;
      case Command::kFullPrint:
        state::FullPrint(state);
        break;
      case Command::kExit:
        std::cout << "Exiting" << std::endl;
        return 0;
      case Command::kFill:
        state::FillPencilMarks(state);
        break;
      case Command::kInsert:
        state::InsertValuesFromPencilMarks(state);
        break;
      case Command::kReduce:
        state::ReducePencilMarks(state);
        break;
      case Command::kReducePairs:
        state::ReducePairs(state);
        break;
      case Command::kIter:
        state::ReducePencilMarks(state);
        if (state.changed_on_iter != state.current_iter) {
          state::ReducePairs(state);
        }
        if (state.changed_on_iter != state.current_iter) {
          std::cout << std::endl;
          state::FullPrint(state);
          std::cout << "Cannot reduce further, try using brute force"
                    << std::endl;
        }
        state::InsertValuesFromPencilMarks(state);
        state::CheckIfSolved(state);
        break;
      default:
        std::cout << "Unknown command, for exit type 'q'" << std::endl;
        break;
    }
    command = GetNextCommand();
  }
  return 0;
}
