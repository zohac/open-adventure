import 'dart:math';
import '../../domain/services/random_number_generator.dart'; // Interface du domaine

class RandomNumberGeneratorImpl implements RandomNumberGenerator {
  @override
  int generate(int seed) {
    return Random().nextInt(seed);
  }
}
