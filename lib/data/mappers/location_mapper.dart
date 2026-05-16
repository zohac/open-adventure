import 'package:open_adventure/data/models/location_model.dart';
import 'package:open_adventure/domain/entities/location.dart';

class LocationMapper {
  static Location toEntity(LocationModel model) {
    return Location(
      id: model.id, // Assurez-vous que `id` dans Location correspond à LocationRefs
      name: model.name,
      description: model.description,
      shortDescription: model.shortDescription,
      items: model.items,
      exits: model.exits,
    );
  }
}
