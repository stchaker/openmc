#include "openmc/transient_source.h"
#include "openmc/simulation.h"
#include "openmc/mesh.h"

#include <catch2/catch_test_macros.hpp>

#include <variant>

using namespace openmc; 

TEST_CASE("Test reading in of transient source") 
{
    read_transient_source("/test_files/transient_source.h5"); 

    // Test requires the correct reading of values into the simulation parameters needed for a kinetic run.
    REQUIRE(simulation::k_t == 0.9992206405083948); 
    REQUIRE(!simulation::precursor_concentrations.empty());
    REQUIRE(simulation::neutron_census.size() == 10000); 
    REQUIRE(std::holds_alternative<SphericalMesh>(simulation::precursor_mesh)); 
}