#include "vcpkg_Commands.h"
#include "vcpkg.h"
#include "vcpkg_System.h"

namespace vcpkg
{
    static const std::string OPTION_PURGE = "--purge";

    static void delete_directory(const fs::path& directory)
    {
        std::error_code ec;
        fs::remove_all(directory, ec);
        if (!ec)
        {
            System::println(System::color::success, "Cleaned up %s", directory.string());
        }
        if (fs::exists(directory))
        {
            System::println(System::color::warning, "Some files in %s were unable to be removed. Close any editors operating in this directory and retry.", directory.string());
        }
    }

    void remove_command(const vcpkg_cmd_arguments& args, const vcpkg_paths& paths, const triplet& default_target_triplet)
    {
        const std::unordered_set<std::string> options = args.check_and_get_optional_command_arguments({OPTION_PURGE});
        auto status_db = database_load_check(paths);

        std::vector<package_spec> specs = args.parse_all_arguments_as_package_specs(default_target_triplet);
        bool alsoRemoveFolderFromPackages = options.find(OPTION_PURGE) != options.end();

        for (const package_spec& spec : specs)
        {
            deinstall_package(paths, spec, status_db);

            if (alsoRemoveFolderFromPackages)
            {
                const fs::path spec_package_dir = paths.packages / spec.dir();
                delete_directory(spec_package_dir);
            }
        }
        exit(EXIT_SUCCESS);
    }
}
