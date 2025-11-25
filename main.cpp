// Minimal IPOPT example: minimize (x-2)^2 subject to x >= 0
// This is the simplest possible IPOPT problem with one variable

#include <IpIpoptApplication.hpp>
#include <IpTNLP.hpp>
#include <iostream>

using namespace Ipopt;

class MinimalNLP : public TNLP
{
  public:
    MinimalNLP() = default;
    virtual ~MinimalNLP() = default;

    // Returns problem dimensions
    bool get_nlp_info(Index &n, Index &m, Index &nnz_jac_g, Index &nnz_h_lag, IndexStyleEnum &index_style) override
    {
        n = 1;         // One variable: x
        m = 0;         // No constraints
        nnz_jac_g = 0; // No constraint Jacobian entries
        nnz_h_lag = 1; // One Hessian entry
        index_style = TNLP::C_STYLE;
        return true;
    }

    // Returns variable bounds
    bool get_bounds_info(Index n, Number *x_l, Number *x_u, Index m, Number *g_l, Number *g_u) override
    {
        x_l[0] = 0.0;  // x >= 0
        x_u[0] = 1e19; // No upper bound
        return true;
    }

    // Returns starting point
    bool get_starting_point(Index n, bool init_x, Number *x, bool init_z, Number *z_L, Number *z_U, Index m,
                            bool init_lambda, Number *lambda) override
    {
        if (init_x)
        {
            x[0] = 5.0; // Start at x=5
        }
        return true;
    }

    // Returns objective value: f(x) = (x-2)^2
    bool eval_f(Index n, const Number *x, bool new_x, Number &obj_value) override
    {
        obj_value = (x[0] - 2.0) * (x[0] - 2.0);
        return true;
    }

    // Returns gradient of objective: f'(x) = 2(x-2)
    bool eval_grad_f(Index n, const Number *x, bool new_x, Number *grad_f) override
    {
        grad_f[0] = 2.0 * (x[0] - 2.0);
        return true;
    }

    // Returns constraint values (none in this problem)
    bool eval_g(Index n, const Number *x, bool new_x, Index m, Number *g) override
    {
        return true;
    }

    // Returns constraint Jacobian structure and values (none)
    bool eval_jac_g(Index n, const Number *x, bool new_x, Index m, Index nele_jac, Index *iRow, Index *jCol,
                    Number *values) override
    {
        return true;
    }

    // Returns Hessian of Lagrangian: H = 2 (constant)
    bool eval_h(Index n, const Number *x, bool new_x, Number obj_factor, Index m, const Number *lambda, bool new_lambda,
                Index nele_hess, Index *iRow, Index *jCol, Number *values) override
    {
        if (values == nullptr)
        {
            // Return structure
            iRow[0] = 0;
            jCol[0] = 0;
        }
        else
        {
            // Return values
            values[0] = obj_factor * 2.0;
        }
        return true;
    }

    // Called when optimization finishes
    void finalize_solution(SolverReturn status, Index n, const Number *x, const Number *z_L, const Number *z_U, Index m,
                           const Number *g, const Number *lambda, Number obj_value, const IpoptData *ip_data,
                           IpoptCalculatedQuantities *ip_cq) override
    {
        std::cout << "\n=== Solution ===" << std::endl;
        std::cout << "x = " << x[0] << std::endl;
        std::cout << "f(x) = " << obj_value << std::endl;
        std::cout << "Expected: x = 2.0, f(x) = 0.0" << std::endl;
    }
};

int main()
{
    std::cout << "IPOPT Minimal Example" << std::endl;
    std::cout << "Minimize: (x-2)^2" << std::endl;
    std::cout << "Subject to: x >= 0" << std::endl;
    std::cout << "Starting point: x = 5" << std::endl;

    SmartPtr<TNLP> mynlp = new MinimalNLP();
    SmartPtr<IpoptApplication> app = IpoptApplicationFactory();

    app->Options()->SetIntegerValue("print_level", 5);
    app->Options()->SetStringValue("linear_solver", "mumps");
    app->Options()->SetStringValue("mu_strategy", "adaptive");

    ApplicationReturnStatus status = app->Initialize();
    if (status != Solve_Succeeded)
    {
        std::cerr << "IPOPT initialization failed!" << std::endl;
        return 1;
    }

    status = app->OptimizeTNLP(mynlp);
    if (status == Solve_Succeeded)
    {
        std::cout << "\nOptimization succeeded!" << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "\nOptimization failed with status " << status << std::endl;
        return 1;
    }
}