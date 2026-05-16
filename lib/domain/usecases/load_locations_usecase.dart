import 'package:open_adventure/domain/entities/location.dart';
import 'package:open_adventure/domain/repositories/location_repository.dart';

class LoadLocationsUseCase {
  final LocationRepository locationRepository;

  LoadLocationsUseCase(this.locationRepository);

  Future<List<Location>> execute() async {
    return await locationRepository.getAllLocations();
  }
}
