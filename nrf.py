import os
import json
import sys

user_commands = (
    "build",
    "flash",
    "cmsis"
)


class FileConfig:

    def __init__(self, json_config):

        self.json_config = json_config
        self.sdkRoot = ""
        self.sdkBuildDir = ""
        self.softDevice = ""
        self.projectTarget = ""
        self.projectHex = ""
        self.projectBuildDir = ""
        self.mergedOutput = ""
        self.cmsisPath = ""
        self.sdkConfPath = ""

        self.set_conf_attr()

    def set_conf_attr(self):

        for name, attribute in self.json_config.items():
            setattr(self, name, attribute)

    def print_conf(self):

        print(self.sdkRoot)
        print(self.sdkBuildDir)
        print(self.sdkRoot)
        print(self.sdkRoot)
        print(self.projectTarget)
        print(self.projectHex)


class NrfBuilder(FileConfig):

    def __init__(self, json_config):
        FileConfig.__init__(self, json_config)
        self.tag = "Nrf Builder"
        self.cmsisCmd = "java -jar " + self.cmsisPath + " " + self.sdkConfPath
        self.command_dict = {
            "build": "ninja " + self.projectTarget,
            "merge": "ninja merge_" + self.projectTarget,
            "flash": "ninja flash_" + self.projectTarget,
            "config": "java -jar " + self.cmsisPath + " " + self.sdkConfPath
        }
        self.command_list = [
            "ninja " + self.projectTarget,
            "mergehex -m " + self.softDevice + " " + self.projectHex +
            " -o " + self.mergedOutput,
            "nrfjprog --eraseall",
            "nrfjprog --program " + self.mergedOutput,
            "nrfjprog --verify " + self.mergedOutput + " --fast",
            'nrfjprog --reset',

        ]

        self.command_description = [
            "attempting to run ninja and perform build step",
            "attempting to merge hex",
            "attempting to erase all",
            "attempting to program",
            "attempting to verify",
            "attempting to reset"
        ]

    def print_cmd_description(self, descpription_idx):
        print(self.tag + ":" + self.command_description[descpription_idx])

    def disp_dir(self):
        print(os.listdir(os.getcwd()))

    def print_command(self, command_idx):
        print(self.command_list[command_idx])

    def print_all_commands(self):
        for i in self.command_list:
            print(i)

    def build_project(self):
        os.chdir(self.sdkBuildDir)
        self.print_cmd_description(0)
        # os.system(self.command_list[0])
        os.system(self.command_dict["build"])

    def merge_project(self):
        os.chdir(self.sdkBuildDir)
        os.system(self.command_dict["merge"])

    def flash_project(self):
        os.chdir(self.sdkBuildDir)
        os.system(self.command_dict["flash"])


    def run_cmsis_config(self):
        os.system(self.command_dict["config"])


def main():

    total_args = len(sys.argv)
    sys.argv = [element.lower() for element in sys.argv]

    commands_passed = total_args > 1
    json_config_file = "build_flash_config.json"

    json_config = None
    if os.path.exists(json_config_file) is False:
        print("There is no build_flash_config.json in this directory!")
        exit()

    with open(json_config_file, "r") as read_file:
        json_config = json.load(read_file)

    nrf_builder = NrfBuilder(json_config)

    if commands_passed is False:
        nrf_builder.build_project()
        nrf_builder.merge_project()
        nrf_builder.flash_project()
        exit()

    if "build" in sys.argv:
        nrf_builder.build_project()
        exit()

    if "merge" in sys.argv:
        nrf_builder.merge_project()
        exit()

    if "flash" in sys.argv:
        nrf_builder.flash_project()
        exit()
    
    if "config" in sys.argv:
        nrf_builder.run_cmsis_config()
        exit()
    
    print("Command given is not recognized")


if __name__ == "__main__":
    main()
