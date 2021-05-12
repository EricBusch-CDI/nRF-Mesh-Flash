import os
import json
# import sys
class FileConfig:

    def __init__(self, json_config):

        self.json_config = json_config
        self.sdkRoot = ""
        self.sdkBuildDir = ""
        self.softDevice = ""
        self.projectName = ""
        self.projectDir = ""
        self.projectDebugDir = ""
        self.mergedOutput = ""
        self.set_conf_attr()

    def set_conf_attr(self):
        for name, attribute in self.json_config.items():
            setattr(self, name, attribute)

    def print_conf(self):

        print(self.sdkRoot)
        print(self.sdkBuildDir)
        print(self.sdkRoot)
        print(self.sdkRoot)
        print(self.projectName)



class NrfBuilder(FileConfig):

    def __init__(self, json_config):
        FileConfig.__init__(self, json_config)
        self.tag = "Nrf Builder"
        
        self.command_list = [
            "ninja " + self.projectName,
            "mergehex -m " + self.softDevice + " " + self.projectName +
            " -o " + self.mergedOutput,
            "nrfjprog --eraseall",
            "nrfjprog --program " + self.mergedOutput,
            "nrfjprog --verify " + self.mergedOutput + " --fast",
            'nrfjprog --reset'
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

    def print_command(self,command_idx):
        print(self.command_list[command_idx])

    def print_all_commands(self):
        for i in self.command_list:
            print(i)

    def build_project(self):
        os.chdir(self.sdkBuildDir)
        self.print_cmd_description(0)
        os.system(self.command_list[0])

    def flash_project(self):
        os.chdir(self.projectDebugDir)
        self.disp_dir()
        for i in range(1, len(self.command_list)):
            self.print_cmd_description(i)
            os.system(self.command_list[i])


def main():
    
    json_config_file = "build_flash_config.json"

    json_config = None
    with open(json_config_file, "r") as read_file:
        json_config = json.load(read_file)


    nrf_builder = NrfBuilder(json_config)
   
    nrf_builder.build_project()
    nrf_builder.flash_project()


if __name__ == "__main__":
    main()
