//
// Created by omar on 17-9-15.
//

#include "Scene.h"

namespace Kernel {
    Scene::Scene(std::shared_ptr<PSTDFileConfiguration> config) {
        this->config = config;
        this->settings = std::make_shared<PSTDFileSettings>(config->Settings);
    }

    void Scene::add_pml_domains() {

    }

    void Scene::add_domain(std::shared_ptr<Domain> domain) {
        for (int i = 0; i < this->domain_list.size(); i++) {
            std::shared_ptr<Domain> other_domain = domain_list.at(
                    i); // Todo: Gives parameter type mismatch error; important?
            if (domain->is_sec_pml && other_domain->is_sec_pml) {
                // Cannot interact, since no secondary PML domains are adjacent
                continue;
            } else if (domain->is_sec_pml && !other_domain->is_pml) {
                // Cannot interact, since a regular domain does not touch a secondary PML domain
                continue;
            } else if (other_domain->is_sec_pml && !domain->is_pml) {
                // Cannot interact, since a regular domain does not touch a secondary PML domain
                continue;
            }
            if (domain->is_pml && other_domain->is_pml) {
                bool pml_for_domain = other_domain->is_pml_for(domain);
                bool pml_for_other_domain = domain->is_pml_for(other_domain);
                if ((domain->is_sec_pml && pml_for_other_domain) || (other_domain->is_sec_pml && pml_for_domain)) {
                    // Important case: Domain is pml for second domain. Pass
                } else if (other_domain->pml_for_domain_list.size() != 1 || domain->pml_for_domain_list.size() != 1) {
                    // One is PML for multiple domains. Continue? Todo...
                    continue;
                } else if (!other_domain->pml_for_domain_list.at(0)->is_neighbour_of(
                        domain->pml_for_domain_list.at(0))) {
                    //Todo: Why just check the first one?
                    continue;
                }
                else if (!domain->pml_for_domain_list.at(0)->is_neighbour_of(other_domain->pml_for_domain_list.at(0))) {
                    //Same
                    continue;
                }
                if (domain->is_sec_pml != other_domain->is_sec_pml) {
                    //
                    continue;
                }
            }
            std::shared_ptr<Boundary> boundary = nullptr;
            std::string orientation = "";
            //Boundary range checking.

            //Adding domains and neigbours
            //Todo (Omar) Finish
        }
    }


    void Scene::add_receiver(const float x, const float y, const float z) {
        std::vector<float> grid_like_location = {x, y, z};
        std::shared_ptr<Domain> container(nullptr);
        for (int i = 0; i < this->domain_list.size(); i++) {
            std::shared_ptr<Domain> domain = this->domain_list.at(i);
            if (domain->is_pml && domain->contains_location(grid_like_location)) {
                container = domain;
            }
        }
        assert(container != nullptr);
        int id = receiver_list.size() + 1;
        std::shared_ptr<Receiver> receiver(new Receiver(grid_like_location, this->settings, id, container));
        this->receiver_list.push_back(receiver);
    }

    void Scene::add_speaker(const float x, const float y, const float z) {
        // Do not really need to be on the heap. Doing it now for consistency with Receiver.
        std::vector<float> grid_like_location = {x, y, z};
        std::shared_ptr<Speaker> speaker(new Speaker(grid_like_location));
        for (int i = 0; i < this->domain_list.size(); i++) {
            speaker->addDomainContribution(this->domain_list.at(i));
        }
        this->speaker_list.push_back(speaker);
    }

    void Scene::compute_rho_matrices() {
        for (int i = 0; i < this->domain_list.size(); i++) {
            this->domain_list.at(i)->calc_rho_matrices();
        }
    }


}