//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date:
//
//
// Authors:
//
//
//////////////////////////////////////////////////////////////////////////

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <fstream>

#include <kernel/PSTDKernel.h>
#include <kernel/MockKernel.h>

#include <boost/regex.hpp>
#include <shared/PSTDFile.h>
#include <shared/export/Image.h>

#include "edit.h"
#include "output.h"
#include "main.h"

namespace OpenPSTD
{
    namespace CLI
    {

        namespace po = boost::program_options;
        using namespace boost;
        using namespace boost::program_options;


        std::string CreateCommand::GetName()
        {
            return "create";
        }

        std::string CreateCommand::GetDescription()
        {
            return "Create an input file, see OpenPSTD-cli create -h";
        }

        int CreateCommand::execute(int argc, const char **argv)
        {
            po::variables_map vm;

            try
            {
                po::options_description desc("Allowed options");
                desc.add_options()
                        ("help,h", "produce help message")
                        ("scene-file,f", po::value<std::string>(), "The scene file that has to be used (required)");

                po::positional_options_description p;
                p.add("scene-file", 1);

                po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
                po::notify(vm);

                if (vm.count("scene-file") == 0)
                {
                    std::cerr << "scene file is required" << std::endl;
                    std::cout << desc << std::endl;
                    return 1;
                }

                std::string filename = vm["scene-file"].as<std::string>();

                Shared::PSTDFile::New(filename);
                return 0;
            }
            catch (std::exception &e)
            {
                std::cerr << "error: " << e.what() << "\n";
                return 1;
            }
            catch (...)
            {
                std::cerr << "Exception of unknown type!\n";
                return 1;
            }
        }

        std::string ListCommand::GetName()
        {
            return "list";
        }

        std::string ListCommand::GetDescription()
        {
            return "Lists the data in an input file, see OpenPSTD-cli list -h";
        }

        int ListCommand::execute(int argc, const char **argv)
        {
            po::variables_map vm;

            try
            {
                po::options_description desc("Allowed options");
                desc.add_options()
                        ("help,h", "produce help message")
                        ("scene-file,f", po::value<std::string>(), "The scene file that has to be used (required)")
                        ("debug,d", "debug information(only useful for development)");

                po::positional_options_description p;
                p.add("scene-file", 1);

                po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
                po::notify(vm);

                if (vm.count("scene-file") == 0)
                {
                    std::cerr << "scene file is required" << std::endl;
                    std::cout << desc << std::endl;
                    return 1;
                }

                std::string filename = vm["scene-file"].as<std::string>();

                Print(filename, vm.count("debug") > 0);
                return 0;
            }
            catch (std::exception &e)
            {
                std::cerr << "error: " << e.what() << "\n";
                return 1;
            }
            catch (...)
            {
                std::cerr << "Exception of unknown type!\n";
                return 1;
            }
        }

        void ListCommand::Print(const std::string &filename, bool debug)
        {
            std::unique_ptr<Shared::PSTDFile> file = Shared::PSTDFile::Open(filename);

            if (debug)
            {
                std::cout << "=============================================================" << std::endl;
                std::cout << "== Debug                                                   ==" << std::endl;
                std::cout << "=============================================================" << std::endl;

                file->OutputDebugInfo();
            }

            std::cout << "=============================================================" << std::endl;
            std::cout << "== Configuration                                           ==" << std::endl;
            std::cout << "=============================================================" << std::endl;
            auto SceneConf = file->GetSceneConf();
            std::cout << "Settings: " << std::endl;
            std::cout << "  Grid spacing: " << SceneConf->Settings.GetGridSpacing() << std::endl;
            std::cout << "  Patche rror: " << SceneConf->Settings.GetPatchError() << std::endl;
            std::cout << "  Window size: " << SceneConf->Settings.GetWindowSize() << std::endl;
            std::cout << "  Render time: " << SceneConf->Settings.GetRenderTime() << std::endl;
            std::cout << "  PML cells: " << SceneConf->Settings.GetPMLCells() << std::endl;
            std::cout << "  Attenuation of PML cells: " << SceneConf->Settings.GetAttenuationOfPMLCells() << std::endl;
            std::cout << "  Density of air: " << SceneConf->Settings.GetDensityOfAir() << std::endl;
            std::cout << "  Max frequency: " << SceneConf->Settings.GetMaxFrequency() << std::endl;
            std::cout << "  Sound speed: " << SceneConf->Settings.GetSoundSpeed() << std::endl;
            std::cout << "  FactRK: " << SceneConf->Settings.GetFactRK() << std::endl;
            std::cout << "  Save Nth: " << SceneConf->Settings.GetSaveNth() << std::endl;
            std::cout << "  Bandwidth: " << SceneConf->Settings.GetBandWidth() << std::endl;
            std::cout << "  Spectral interpolation: " << SceneConf->Settings.GetSpectralInterpolation() << std::endl;
            std::cout << "  Wave length: " << SceneConf->Settings.GetWaveLength() << std::endl;
            std::cout << "  Time step: " << SceneConf->Settings.GetTimeStep() << std::endl;
            std::cout << "  RKCoefficients: ";
            auto coef = SceneConf->Settings.GetRKCoefficients();
            for (int i = 0; i < coef.size(); ++i)
            {
                std::cout << coef[i] << " ";
            }
            std::cout << std::endl;
            std::cout << "  Window: " << SceneConf->Settings.GetWindow() << std::endl;

            std::cout << std::endl;
            std::cout << "Speakers: " << std::endl;
            for (int i = 0; i < SceneConf->Speakers.size(); ++i)
            {
                std::cout << "  " << i << ": <" << SceneConf->Speakers[i].x() << ", " << SceneConf->Speakers[i].y() <<
                ">" << std::endl;
            }

            std::cout << std::endl;
            std::cout << "Receivers: " << std::endl;
            for (int i = 0; i < SceneConf->Receivers.size(); ++i)
            {
                std::cout << "  " << i << ": <" << SceneConf->Receivers[i].x() << ", " << SceneConf->Receivers[i].y() <<
                ">" << std::endl;
            }

            std::cout << std::endl;
            std::cout << "Domains: " << std::endl;
            for (int i = 0; i < SceneConf->Domains.size(); ++i)
            {
                std::cout << "  Domain " << i << ":" << std::endl;
                std::cout << "    Pos: <" << SceneConf->Domains[i].TopLeft.x() << ", " <<
                SceneConf->Domains[i].TopLeft.y() << ">" << std::endl;
                std::cout << "    Size: <" << SceneConf->Domains[i].Size.x() << ", " <<
                SceneConf->Domains[i].Size.y() << ">" << std::endl;
                std::cout << "    Top: Absorption " << SceneConf->Domains[i].T.Absorption << ", LR " <<
                SceneConf->Domains[i].T.LR << std::endl;
                std::cout << "    Bottom: Absorption " << SceneConf->Domains[i].B.Absorption << ", LR " <<
                SceneConf->Domains[i].B.LR << std::endl;
                std::cout << "    Left: Absorption " << SceneConf->Domains[i].L.Absorption << ", LR " <<
                SceneConf->Domains[i].L.LR << std::endl;
                std::cout << "    Right: Absorption " << SceneConf->Domains[i].R.Absorption << ", LR " <<
                SceneConf->Domains[i].R.LR << std::endl;
            }

            std::cout << "=============================================================" << std::endl;
            std::cout << "== Data                                                    ==" << std::endl;
            std::cout << "=============================================================" << std::endl;

            int frameCount = file->GetDomainCount();
            std::cout << "Domains: " << frameCount << std::endl;
            for (int i = 0; i < frameCount; ++i)
            {
                std::cout << "Frame count for domain " << i << ": " << file->GetFrameCount(i) << std::endl;
            }
        }

        std::string EditCommand::GetName()
        {
            return "edit";
        }

        std::string EditCommand::GetDescription()
        {
            return "Edits an input file, see OpenPSTD-cli edit -h";
        }

        int EditCommand::execute(int argc, const char **argv)
        {
            po::variables_map vm;

            std::vector<std::unique_ptr<EditCommandPart>> commands;
            commands.push_back(std::unique_ptr<AddDomainEditCommandPart>(new AddDomainEditCommandPart()));
            commands.push_back(std::unique_ptr<RemoveDomainEditCommandPart>(new RemoveDomainEditCommandPart()));
            commands.push_back(std::unique_ptr<ChangeDomainEditCommandPart>(new ChangeDomainEditCommandPart()));
            commands.push_back(
                    std::unique_ptr<ChangeEdgeAbsorptionEditCommandPart>(new ChangeEdgeAbsorptionEditCommandPart()));
            commands.push_back(std::unique_ptr<ChangeEdgeLREditCommandPart>(new ChangeEdgeLREditCommandPart()));
            commands.push_back(std::unique_ptr<AddSpeakerEditCommandPart>(new AddSpeakerEditCommandPart()));
            commands.push_back(std::unique_ptr<RemoveSpeakerEditCommandPart>(new RemoveSpeakerEditCommandPart()));
            commands.push_back(std::unique_ptr<AddReceiverEditCommandPart>(new AddReceiverEditCommandPart()));
            commands.push_back(std::unique_ptr<RemoveReceiverEditCommandPart>(new RemoveReceiverEditCommandPart()));
            commands.push_back(std::unique_ptr<SetOptionEditCommandPart>(new SetOptionEditCommandPart()));


            try
            {
                po::options_description desc("Allowed options");
                for (int i = 0; i < commands.size(); ++i)
                {
                    commands[i]->AddOptions(desc.add_options());
                }
                desc.add_options()
                        ("help,h",
                         "produce help message")

                        ("scene-file,f",
                         po::value<std::string>(),
                         "The scene file that has to be used (required)");

                po::positional_options_description p;
                p.add("scene-file", 1);

                po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
                po::notify(vm);

                if (vm.count("help"))
                {
                    std::cout << desc << std::endl;
                    return 0;
                }

                if (vm.count("scene-file") == 0)
                {
                    std::cerr << "scene file is required" << std::endl;
                    std::cout << desc << std::endl;
                    return 1;
                }

                std::string filename = vm["scene-file"].as<std::string>();

                //open file
                std::unique_ptr<Shared::PSTDFile> file = Shared::PSTDFile::Open(filename);
                auto conf = file->GetSceneConf();

                for (int i = 0; i < commands.size(); ++i)
                {
                    commands[i]->Execute(conf, vm);
                }

                file->SetSceneConf(conf);
                return 0;
            }
            catch (std::exception &e)
            {
                std::cerr << "error: " << e.what() << "\n";
                return 1;
            }
            catch (...)
            {
                std::cerr << "Exception of unknown type!\n";
                return 1;
            }
        }

        std::string RunCommand::GetName()
        {
            return "run";
        }

        std::string RunCommand::GetDescription()
        {
            return "Run the OpenPSTD algorithm, see OpenPSTD-cli run -h";
        }

        int RunCommand::execute(int argc, const char **argv)
        {
            po::variables_map vm;

            try
            {
                po::options_description desc("Allowed options");
                desc.add_options()
                        ("help,h", "produce help message")
                        ("scene-file,f", po::value<std::string>(), "The scene file that has to be used (required)")
                        ("multithreaded,m", "use the multi-threaded solver (mutually exclusive with gpu accelerated)")
                        ("gpu-accelerated,g",
                         "Use the gpu for the calculations (mutually exclusive with multithreaded)")
                        ("mock,M", "Use the mock kernel(only useful for development)")
                    //("write-plot,p", "Plots are written to the output directory")
                    //("write-array,a", "Arrays are written to the output directory")
                        ;

                po::positional_options_description p;
                p.add("scene-file", 1);

                po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
                po::notify(vm);

                if (vm.count("help"))
                {
                    std::cout << desc << std::endl;
                    return 0;
                }

                if (vm.count("scene-file") == 0)
                {
                    std::cerr << "scene file is required" << std::endl;
                    std::cout << desc << std::endl;
                    return 1;
                }

                if (vm.count("multithreaded") > 0 && vm.count("gpu-accelerated") > 0)
                {
                    std::cerr << "multithreaded and gpu accelerated options are mutually exclusive" << std::endl;
                    std::cout << desc << std::endl;
                    return 1;
                }

                if (vm.count("mock") > 0 && (vm.count("multithreaded") > 0 || vm.count("gpu-accelerated") > 0))
                {
                    std::cout << "warning: no multithreaded or gpu accelerated versions of the mock kernel, "
                            "using normal version" << std::endl;
                }

                std::string filename = vm["scene-file"].as<std::string>();

                //open file (and make a shared_ptr of the unique_ptr)
                std::shared_ptr<Shared::PSTDFile> file = Shared::PSTDFile::Open(filename);
                //get conf for the kernel
                std::shared_ptr<Kernel::PSTDConfiguration> conf = file->GetSceneConf();
                //initilize output in file
                std::cout << "Delete old results(if any)" << std::endl;
                file->DeleteSimulationResults();
                std::cout << "initilize new results" << std::endl;
                file->InitializeSimulationResults(conf->Domains.size());
                //create kernel
                std::unique_ptr<Kernel::KernelInterface> kernel;
                if (vm.count("mock") > 0)
                {
                    //use the mocking
                    kernel = std::unique_ptr<Kernel::MockKernel>(new Kernel::MockKernel());
                }
                else
                {
                    //use the real kernel
                    kernel = std::unique_ptr<Kernel::PSTDKernel>(new Kernel::PSTDKernel());
                }
                //configure the kernel
                kernel->start_kernel(conf);
                //create output
                std::shared_ptr<Kernel::KernelCallback> output = std::make_shared<CLIOutput>(file);
                //run kernel
                kernel->run(output.get());
                return 0;
            }
            catch (std::exception &e)
            {
                std::cerr << "error: " << e.what() << "\n";
                return 1;
            }
            catch (...)
            {
                std::cerr << "Exception of unknown type!\n";
                return 1;
            }
        }

        std::unique_ptr<Shared::ExportDomain> ExportCommand::GetExport(std::string format)
        {
            std::vector<std::unique_ptr<Shared::ExportDomain>> exports;
            exports.push_back(std::unique_ptr<Shared::ExportImage>(new Shared::ExportImage()));

            for (int i = 0; i < exports.size(); ++i)
            {
                auto f = exports[i]->GetFormats();
                for (int j = 0; j < f.size(); ++j)
                {
                    if (f[j].compare(format) == 0)
                        return std::move(exports[i]);
                }
            }
            throw Shared::ExportFormatNotSupported(format, std::vector<std::string>());
        }

        std::string ExportCommand::GetName()
        {
            return "export";
        }

        std::string ExportCommand::GetDescription()
        {
            return "Exports data into other formats, like images or matlab files, see OpenPSTD-cli export -h";
        }

        int ExportCommand::execute(int argc, const char **argv)
        {
            po::variables_map vm;
            std::vector<std::unique_ptr<Shared::ExportDomain>> exports;
            exports.push_back(std::unique_ptr<Shared::ExportImage>(new Shared::ExportImage()));

            try
            {
                po::options_description desc("Allowed options");
                desc.add_options()
                        ("help,h", "produce help message")
                        ("scene-file,f", po::value<std::string>(), "The scene file that has to be used (required)")
                        ("format,F", po::value<std::string>(), "The output format")
                        ("list-format,l", "Prints a list with output formats")
                        ("output-dir,d", po::value<std::string>()->default_value("./"), "output directory")
                        ("output-name,n", po::value<std::string>()->default_value("result"), "prefix name")
                        ("start-frame,s", po::value<int>()->default_value(0),
                         "The start that has to be exported(included)")
                        ("end-frame,e", po::value<int>()->default_value(-1),
                         "The end frame that has to be exported(included), -1 for the last frame.")
                        ("domains,D", po::value<std::vector<int> >(),
                         "Domain that should be exported. By default it exports all domains. Can be used multiple times.");

                po::positional_options_description p;
                p.add("scene-file", 1);

                po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
                po::notify(vm);

                if (vm.count("help"))
                {
                    std::cout << desc << std::endl;
                    return 0;
                }

                if (vm.count("list-format"))
                {
                    std::cout << "Formats supported: " << std::endl;
                    for (int i = 0; i < exports.size(); ++i)
                    {
                        auto f = exports[i]->GetFormats();
                        for (int j = 0; j < f.size(); ++j)
                        {
                            std::cout << f[j] << std::endl;
                        }
                    }
                    return 0;
                }

                if (vm.count("scene-file") == 0)
                {
                    std::cerr << "scene file is required" << std::endl;
                    std::cout << desc << std::endl;
                    return 1;
                }

                if (vm.count("format") == 0)
                {
                    std::cerr << "format is required" << std::endl;
                    std::cout << desc << std::endl;
                    return 1;
                }

                std::string filename = vm["scene-file"].as<std::string>();

                //open file (and make a shared_ptr of the unique_ptr)
                std::shared_ptr<Shared::PSTDFile> file = Shared::PSTDFile::Open(filename);

                std::string format = vm["format"].as<std::string>();
                std::unique_ptr<Shared::ExportDomain> e = GetExport(format);

                std::vector<int> domains;
                if (vm.count("domains") > 0)
                {
                    domains = vm["domains"].as<std::vector<int>>();
                }

                e->ExportData(format, file, vm["output-dir"].as<std::string>(), vm["output-name"].as<std::string>(),
                              false, domains, vm["start-frame"].as<int>(), vm["end-frame"].as<int>());

                return 0;
            }
            catch (std::exception &e)
            {
                std::cerr << "error: " << e.what() << "\n";
                return 1;
            }
            catch (...)
            {
                std::cerr << "Exception of unknown type!\n";
                return 1;
            }
        }
    }
}

int main(int argc, const char *argv[])
{
    using namespace OpenPSTD::CLI;
    std::vector<std::unique_ptr<Command>> commands;
    commands.push_back(std::unique_ptr<CreateCommand>(new CreateCommand()));
    commands.push_back(std::unique_ptr<ListCommand>(new ListCommand()));
    commands.push_back(std::unique_ptr<EditCommand>(new EditCommand()));
    commands.push_back(std::unique_ptr<RunCommand>(new RunCommand()));
    commands.push_back(std::unique_ptr<ExportCommand>(new ExportCommand()));

    if (argc >= 2)
    {
        std::string command = argv[1];
        for (int i = 0; i < commands.size(); ++i)
        {
            if (command.compare(commands[i]->GetName()) == 0)
            {
                return commands[i]->execute(argc - 1, argv + 1);
                //one less argument, specificaly the first, that is the reason of the +1
            }
        }
    }

    std::cout << "Usage: OpenPSTD-cli <command>" << std::endl;
    std::cout << std::endl;
    std::cout << "possible commands are: " << std::endl;
    for (int i = 0; i < commands.size(); ++i)
    {
        std::cout << "\t" << commands[i]->GetName() << "\t\t" << commands[i]->GetDescription() << std::endl;
    }
}