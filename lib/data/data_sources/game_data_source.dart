import 'package:flutter/services.dart';
import 'package:open_adventure/data/models/dwarf_model.dart';
import 'package:yaml/yaml.dart';

import '../models/item_model.dart';
import '../models/location_model.dart';

class GameDataSource {
  Future<Map<int, ItemModel>> loadItems() async {
    final yamlString = await rootBundle.loadString('assets/data/items.yaml');
    final yamlData = loadYaml(yamlString) as YamlList;
    final itemsList = yamlData.map((item) => Map<String, dynamic>.from(item)).toList();
    final items = itemsList.map((item) => ItemModel.fromYaml(item)).toList();
    // Convertir la liste en Map<int, ItemModel>
    return { for (var item in items) item.id : item };
  }

  Future<Map<int, LocationModel>> loadLocations() async {
    final yamlString = await rootBundle.loadString('assets/data/locations.yaml');
    final yamlData = loadYaml(yamlString) as YamlList;
    final locationsList = yamlData.map((loc) => Map<String, dynamic>.from(loc)).toList();
    final locations = locationsList.map((loc) => LocationModel.fromYaml(loc)).toList();
    // Convertir la liste en Map<int, LocationModel>
    return { for (var location in locations) location.id : location };
  }

  // Future<Map<int, DwarfModel>> loadDwarves() async {
  //
  //   // TODO : implement loadDwarves
  // }
}
