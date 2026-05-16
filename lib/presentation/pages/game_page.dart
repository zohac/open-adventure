import 'package:flutter/material.dart';

import '../controllers/game_controller.dart';

class GamePage extends StatefulWidget {
  final GameController gameController;

  GamePage({required this.gameController});

  @override
  _GamePageState createState() => _GamePageState();
}

class _GamePageState extends State<GamePage> {
  final TextEditingController _controller = TextEditingController();
  String _output = '';

  void _handleInput() {
    String input = _controller.text;
    widget.gameController.handleUserInput(input);
    _controller.clear();

    // Mettez à jour l'interface utilisateur en fonction de l'état du jeu
    setState(() {
      // Mettez à jour _output ou d'autres variables
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Open Adventure'),
      ),
      body: Column(
        children: [
          Expanded(
            child: SingleChildScrollView(
              child: Text(_output),
            ),
          ),
          TextField(
            controller: _controller,
            onSubmitted: (value) => _handleInput(),
            decoration: InputDecoration(
              hintText: 'Que voulez-vous faire ?',
            ),
          ),
        ],
      ),
    );
  }
}
