#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

void *load_plugin_facility(const char* path, const plugin_facility_enum_t facility_type, const char* facility_name);

#endif
