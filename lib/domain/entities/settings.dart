class Settings {
  String? logfp;  // Correspond à un fichier de log, NULL en C est équivalent à null en Dart
  bool oldStyle;
  bool prompt;

  Settings({
    this.logfp,
    required this.oldStyle,
    required this.prompt,
  });

  // Méthode pour afficher les paramètres actuels
  @override
  String toString() {
    return 'Settings(logfp: $logfp, oldStyle: $oldStyle, prompt: $prompt)';
  }
}
