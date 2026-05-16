import 'dart:async';
import 'package:open_adventure/data/data_sources/game_data_source.dart';
import 'package:open_adventure/domain/entities/location.dart';
import 'package:open_adventure/domain/repositories/location_repository.dart';

import 'package:open_adventure/data/mappers/location_mapper.dart';

class LocationRepositoryImpl implements LocationRepository {
  final GameDataSource dataSource;

  LocationRepositoryImpl(this.dataSource);

  @override
  Future<List<Location>> getAllLocations() async {
    final locationModels = await dataSource.loadLocations();
    return locationModels.values.map((model) => LocationMapper.toEntity(model)).toList();
  }

  @override
  Future<Location> getLocationById(int id) {
    // TODO: implement getLocationById
    throw UnimplementedError();
  }

  @override
  Future<void> updateLocation(Location location) {
    // TODO: implement updateLocation
    throw UnimplementedError();
  }
}
