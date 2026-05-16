import 'command.dart';

class CommandParser {
  Command parse(String input) {
    List<String> words = input.trim().toLowerCase().split(' ');

    if (words.isEmpty) {
      return Command(verb: 'inconnu');
    }

    String verb = words[0];
    String? object = words.length > 1 ? words.sublist(1).join(' ') : null;

    return Command(verb: verb, object: object);
  }
}
