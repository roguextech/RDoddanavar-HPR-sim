#pragma once

// System libraries
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cstdio>

// External libraries
#include "pybind11/pybind11.h"
#include "pybind11/stl.h" // For std::map; induces overhead, remove if possible
#include "pybind11/numpy.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"
#include "eigen/Eigen/Core"
#include "gsl/ode-initval2/gsl_odeiv2.h"

// Project headers
// <none>

namespace py = pybind11;

// Type aliases
using stateMap    = std::map<std::string, double*>;
using stateMapVec = std::map<std::string, std::vector<double>>;

//---------------------------------------------------------------------------//

class Model
{
    
    public: 

        // Data
        bool isInit = false;
        stateMap* state;
        std::set<Model*> depModels; // std::set enforces unique elements

        // Function(s)
        virtual void update()    = 0; // Pure virtual
        virtual void set_state() = 0; // Pure virtual

        void add_dep(Model* dep)
        {
            // std::set will quietly ignore duplicate elements
            // Should an exception be raised when insertion is skipped?
            depModels.insert(dep);
        }

        void update_deps()
        {
            for (const auto& dep : depModels)
            {
                dep->update();
            }
        }

        void init_state(stateMap* stateIn)
        {
            
            state = stateIn;
            set_state();

            for (const auto& dep : depModels)
            {
                dep->init_state(state);
            }

        }

};

//---------------------------------------------------------------------------//

class Engine : public Model
{
    
    public:

        // Function(s)
        void init(py::array_t<double> timeInit  , 
                  py::array_t<double> thrustInit, 
                  py::array_t<double> massInit  );

        void update() override;
        void set_state() override;

        ~Engine(); // Destructor

    private:

        // Data
        double thrust;
        double mass;

        gsl_spline       *thrustSpline, *massSpline;
        gsl_interp_accel *thrustAcc   , *massAcc   ;

};

//---------------------------------------------------------------------------//

class Geodetic : public Model
{
    public:

        // Data

        // Function(s)
        void init(double phiInit);
        void update() override;
        void set_state() override;

    private:

        // Data
        double gravity;

        double phi;
        double gamE;
        double k;
        double e;
        double a;
        double f;
        double m;

        // Function(s)
        double wgs84(double h);

};

//---------------------------------------------------------------------------//

class EOM : public Model
{

    public:

        void init();
        void update() override;
        void set_state() override;

    private:

        // Data
        Eigen::Vector3d force;  // Force  [N]
        Eigen::Vector3d moment; // Moment [N*m]

        Eigen::Vector3d linAcc; // Linear acceleration [m/s^2]
        Eigen::Vector3d linVel; // Linear velocity     [m/s]
        Eigen::Vector3d linPos; // Linear position     [m]

        Eigen::Vector3d angAcc; // Angular acceleration [rad/s^2]
        Eigen::Vector3d angVel; // Angular velocity     [rad/s]
        Eigen::Vector3d angPos; // Angular position     [rad]

};

//---------------------------------------------------------------------------//

class Flight : public Model
{

    public:

        void init();
        void update() override;
        void set_state() override;

        int ode_update(double t, const double y[], double f[], void *params);

        stateMapVec stateTelem;

        ~Flight(); // Destructor

    private:

        // Data
        double time;
        double massBody;

        // ODE stuff - store all this stuff in a struct?
        gsl_odeiv2_system odeSys;
        gsl_odeiv2_driver* odeDriver;

        size_t odeDim = 2;
        const gsl_odeiv2_step_type* odeMethod = gsl_odeiv2_step_rkf45;

        const double odeHStart = 1e-6;
        const double odeEpsAbs = 1e-6;
        const double odeEpsRel = 0.0;

};