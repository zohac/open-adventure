import '../entities/location.dart';

abstract class LocationRepository {
  Future<List<Location>> getAllLocations();
  Future<Location> getLocationById(int id);
  Future<void> updateLocation(Location location);
}
