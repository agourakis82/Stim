#include "stim/util_top/circuit_flow_generators.h"

#include "gtest/gtest.h"

#include "stim/circuit/circuit.test.h"
#include "stim/gen/gen_surface_code.h"
#include "stim/mem/simd_word.test.h"
#include "stim/util_top/circuit_inverse_qec.h"
#include "stim/util_top/has_flow.h"
#include "stim/util_top/missing_detectors.h"
#include "stim/simulators/error_analyzer.h"

using namespace stim;

TEST_EACH_WORD_SIZE_W(circuit_flow_generators, various, {
    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
        )CIRCUIT")),
        (std::vector<Flow<W>>{}));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            X 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("X -> X"),
            Flow<W>::from_str("Z -> -Z"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            H 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("X -> Z"),
            Flow<W>::from_str("Z -> X"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            M 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> Z xor rec[0]"),
            Flow<W>::from_str("Z -> rec[0]"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            M 0 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> rec[0] xor rec[1]"),
            Flow<W>::from_str("1 -> Z xor rec[1]"),
            Flow<W>::from_str("Z -> rec[1]"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            MXX 2 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> X_X xor rec[0]"),
            Flow<W>::from_str("__X -> __X"),
            Flow<W>::from_str("_X_ -> _X_"),
            Flow<W>::from_str("_Z_ -> _Z_"),
            Flow<W>::from_str("X__ -> __X xor rec[0]"),
            Flow<W>::from_str("Z_Z -> Z_Z"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            MYY 3 1 2 3
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> __YY xor rec[1]"),
            Flow<W>::from_str("1 -> _Y_Y xor rec[0]"),
            Flow<W>::from_str("___Y -> ___Y"),
            Flow<W>::from_str("__Y_ -> ___Y xor rec[1]"),
            Flow<W>::from_str("_XZZ -> _ZZX xor rec[0]"),
            Flow<W>::from_str("_ZZZ -> _ZZZ"),
            Flow<W>::from_str("X___ -> X___"),
            Flow<W>::from_str("Z___ -> Z___"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            MZZ 3 1 2 3
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> __ZZ xor rec[1]"),
            Flow<W>::from_str("1 -> _Z_Z xor rec[0]"),
            Flow<W>::from_str("___Z -> ___Z"),
            Flow<W>::from_str("__Z_ -> ___Z xor rec[1]"),
            Flow<W>::from_str("_XXX -> _XXX"),
            Flow<W>::from_str("_Z__ -> ___Z xor rec[0]"),
            Flow<W>::from_str("X___ -> X___"),
            Flow<W>::from_str("Z___ -> Z___"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            ISWAP 3 1 2 3
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("___X -> _YZ_"),
            Flow<W>::from_str("___Z -> _Z__"),
            Flow<W>::from_str("__X_ -> __ZY"),
            Flow<W>::from_str("__Z_ -> ___Z"),
            Flow<W>::from_str("_X__ -> -_ZXZ"),
            Flow<W>::from_str("_Z__ -> __Z_"),
            Flow<W>::from_str("X___ -> X___"),
            Flow<W>::from_str("Z___ -> Z___"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            S 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("X -> Y"),
            Flow<W>::from_str("Z -> Z"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            S_DAG 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("X -> -Y"),
            Flow<W>::from_str("Z -> Z"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            SPP Z0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("X -> Y"),
            Flow<W>::from_str("Z -> Z"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            SQRT_X 0
            S 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("X -> Y"),
            Flow<W>::from_str("Z -> X"),
        }));
    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            SPP X0 Z0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("X -> Y"),
            Flow<W>::from_str("Z -> X"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            SPP X0*X1
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("_X -> _X"),
            Flow<W>::from_str("_Z -> -XY"),
            Flow<W>::from_str("X_ -> X_"),
            Flow<W>::from_str("Z_ -> -YX"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            SPP_DAG Z0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("X -> -Y"),
            Flow<W>::from_str("Z -> Z"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            M 0
            CX rec[-1] 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> Z"),
            Flow<W>::from_str("Z -> rec[0]"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            R 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> Z"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            MR 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> Z"),
            Flow<W>::from_str("Z -> rec[0]"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            M 0
            XCZ 0 rec[-1]
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> Z"),
            Flow<W>::from_str("Z -> rec[0]"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            MPAD 0 1 1 0
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> rec[0]"),
            Flow<W>::from_str("1 -> rec[3]"),
            Flow<W>::from_str("1 -> -rec[1]"),
            Flow<W>::from_str("1 -> -rec[2]"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            M 0
            CY rec[-1] 1
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> Z_ xor rec[0]"),
            Flow<W>::from_str("_X -> _X xor rec[0]"),
            Flow<W>::from_str("_Z -> _Z xor rec[0]"),
            Flow<W>::from_str("Z_ -> rec[0]"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            HERALDED_ERASE(0.04) 1
            HERALDED_PAULI_CHANNEL_1(0.01, 0.02, 0.03, 0.04) 1
            TICK
            MPP X0*Y1*Z2 Z0*Z1
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> rec[0]"),
            Flow<W>::from_str("1 -> rec[1]"),
            Flow<W>::from_str("1 -> XYZ xor rec[2]"),
            Flow<W>::from_str("1 -> ZZ_ xor rec[3]"),
            Flow<W>::from_str("__Z -> __Z"),
            Flow<W>::from_str("_ZX -> _ZX"),
            Flow<W>::from_str("XXX -> _ZY xor rec[2]"),
            Flow<W>::from_str("Z_X -> _ZX xor rec[3]"),
        }));

    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit(R"CIRCUIT(
            MPP Y0*Y1 Y2*Y3
        )CIRCUIT")),
        (std::vector<Flow<W>>{
            Flow<W>::from_str("1 -> __YY xor rec[1]"),
            Flow<W>::from_str("1 -> YY__ xor rec[0]"),
            Flow<W>::from_str("___Y -> ___Y"),
            Flow<W>::from_str("__XZ -> __ZX xor rec[1]"),
            Flow<W>::from_str("__ZZ -> __ZZ"),
            Flow<W>::from_str("_Y__ -> _Y__"),
            Flow<W>::from_str("XZ__ -> ZX__ xor rec[0]"),
            Flow<W>::from_str("ZZ__ -> ZZ__"),
        }));
})

TEST_EACH_WORD_SIZE_W(circuit_flow_generators, repeated_measure_reset, {
    for (const auto &[gate, axis] :
         std::vector<std::pair<std::string, std::string>>{{"MR", "Z"}, {"MRX", "X"}, {"MRY", "Y"}}) {
        for (const auto &targets : std::vector<std::string>{"0 0", "!0 0", "0 !0", "!0 !0"}) {
            SCOPED_TRACE(gate + " " + targets);
            bool first_inverted = targets[0] == '!';
            bool second_inverted = targets[targets.find(' ') + 1] == '!';
            std::vector<Flow<W>> expected{
                Flow<W>::from_str("1 -> " + axis),
                Flow<W>::from_str(std::string("1 -> ") + (second_inverted ? "-" : "") + "rec[1]"),
                Flow<W>::from_str(axis + " -> " + (first_inverted ? "-" : "") + "rec[0]"),
            };
            std::sort(expected.begin(), expected.end());
            EXPECT_EQ(circuit_flow_generators<W>(Circuit(gate + " " + targets)), expected);
        }

        Circuit bundled(gate + " 0 1 !0 !1 0");
        Circuit split(
            gate + " 0\nTICK\n" + gate + " 1\nTICK\n" + gate + " !0\nTICK\n" + gate + " !1\nTICK\n" + gate + " 0");
        EXPECT_EQ(circuit_flow_generators<W>(bundled), circuit_flow_generators<W>(split));

        std::vector<Flow<W>> disjoint_expected{
            Flow<W>::from_str("1 -> " + axis + "_"),
            Flow<W>::from_str("1 -> _" + axis),
            Flow<W>::from_str(axis + "_ -> rec[0]"),
            Flow<W>::from_str("_" + axis + " -> rec[1]"),
        };
        std::sort(disjoint_expected.begin(), disjoint_expected.end());
        Circuit disjoint(gate + " 0 1");
        EXPECT_EQ(circuit_flow_generators<W>(disjoint), disjoint_expected);
        EXPECT_EQ(
            solve_for_flow_measurements<W>(
                disjoint,
                std::vector<Flow<W>>{Flow<W>::from_str(axis + "_ -> 1"), Flow<W>::from_str("_" + axis + " -> 1")}),
            (std::vector<std::optional<std::vector<int32_t>>>{{std::vector<int32_t>{0}}, {std::vector<int32_t>{1}}}));

        const std::string prefix = "H 0\nS 1\nCX 0 1\n";
        EXPECT_EQ(
            circuit_flow_generators<W>(Circuit(prefix + gate + " 0 1 !0 !1 0")),
            circuit_flow_generators<W>(Circuit(
                prefix + gate + " 0\nTICK\n" + gate + " 1\nTICK\n" + gate + " !0\nTICK\n" + gate + " !1\nTICK\n" +
                gate + " 0")));

        Circuit circuit(gate + " 0 0");
        std::vector<Flow<W>> requested{Flow<W>::from_str(axis + " -> 1")};
        auto solved = solve_for_flow_measurements<W>(circuit, requested);
        ASSERT_EQ(solved.size(), 1);
        ASSERT_TRUE(solved[0].has_value());
        requested[0].measurements = *solved[0];
        EXPECT_EQ(check_if_circuit_has_unsigned_stabilizer_flows<W>(circuit, requested), (std::vector<bool>{true}));
    }
})

TEST_EACH_WORD_SIZE_W(circuit_flow_generators, asymmetric_padding, {
    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit("MPAD 1 0")),
        (std::vector<Flow<W>>{Flow<W>::from_str("1 -> rec[1]"), Flow<W>::from_str("1 -> -rec[0]")}));
    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit("MPAD 0 1")),
        (std::vector<Flow<W>>{Flow<W>::from_str("1 -> rec[0]"), Flow<W>::from_str("1 -> -rec[1]")}));
    EXPECT_EQ(
        circuit_flow_generators<W>(Circuit("MPAD 1 0 0")),
        circuit_flow_generators<W>(Circuit("MPAD 1\nTICK\nMPAD 0\nTICK\nMPAD 0")));
})

TEST_EACH_WORD_SIZE_W(circuit_flow_generators, all_operations, {
    auto circuit = generate_test_circuit_with_all_operations();
    auto generators = circuit_flow_generators<W>(circuit);
    auto rng = externally_seeded_rng();
    auto passes = sample_if_circuit_has_stabilizer_flows<W>(256, rng, circuit, generators);
    for (size_t k = 0; k < passes.size(); k++) {
        EXPECT_TRUE(passes[k]) << k << ": " << generators[k];
    }
})

TEST(circuit_flow_generators, repeated_reset_detector_completion) {
    Circuit circuit("H 0\nMR 0 0");
    Circuit detectors = missing_detectors(circuit, true);
    EXPECT_EQ(detectors, Circuit("DETECTOR rec[-1]"));
    circuit += detectors;
    EXPECT_NO_THROW(ErrorAnalyzer::circuit_to_detector_error_model(circuit, false, false, false, 0, false, false));
    Circuit invalid("H 0\nMR 0 0\nDETECTOR rec[-2] rec[-1]");
    EXPECT_THROW(
        ErrorAnalyzer::circuit_to_detector_error_model(invalid, false, false, false, 0, false, false),
        std::invalid_argument);
}

TEST_EACH_WORD_SIZE_W(solve_for_flow_measurements, empty, {
    EXPECT_EQ(
        solve_for_flow_measurements<W>(
            Circuit(R"CIRCUIT(
            )CIRCUIT"),
            (std::vector<Flow<W>>{})),
        (std::vector<std::optional<std::vector<int32_t>>>{}));
})

TEST_EACH_WORD_SIZE_W(solve_for_flow_measurements, simple, {
    EXPECT_EQ(
        solve_for_flow_measurements<W>(
            Circuit(R"CIRCUIT(
                MX 0
            )CIRCUIT"),
            (std::vector<Flow<W>>{
                Flow<W>::from_str("1 -> X0"),
            })),
        (std::vector<std::optional<std::vector<int32_t>>>{
            {std::vector<int32_t>{0}},
        }));

    EXPECT_EQ(
        solve_for_flow_measurements<W>(
            Circuit(R"CIRCUIT(
                MX 0
            )CIRCUIT"),
            (std::vector<Flow<W>>{
                Flow<W>::from_str("1 -> Y0"),
            })),
        (std::vector<std::optional<std::vector<int32_t>>>{
            {},
        }));

    EXPECT_EQ(
        solve_for_flow_measurements<W>(
            Circuit(R"CIRCUIT(
                MX 0
            )CIRCUIT"),
            (std::vector<Flow<W>>{
                Flow<W>::from_str("1 -> X0"),
                Flow<W>::from_str("Y0 -> Y0"),
                Flow<W>::from_str("X0 -> 1"),
                Flow<W>::from_str("X0 -> Z0"),
                Flow<W>::from_str("Y1 -> Y1"),
            })),
        (std::vector<std::optional<std::vector<int32_t>>>{
            {std::vector<int32_t>{0}},
            {},
            {std::vector<int32_t>{0}},
            {},
            {std::vector<int32_t>{}},
        }));

    EXPECT_THROW(
        { solve_for_flow_measurements<W>(Circuit(), (std::vector<Flow<W>>{Flow<W>::from_str("1 -> 1")})); },
        std::invalid_argument);
})

TEST_EACH_WORD_SIZE_W(solve_for_flow_measurements, rep_code, {
    EXPECT_EQ(
        solve_for_flow_measurements<W>(
            Circuit(R"CIRCUIT(
                R 1 3
                CX 0 1 2 3
                CX 4 3 2 1
                M 1 3
            )CIRCUIT"),
            (std::vector<Flow<W>>{
                Flow<W>::from_str("Z0*Z2 -> 1"),
                Flow<W>::from_str("1 -> Z2*Z4"),
                Flow<W>::from_str("1 -> Z0*Z4"),
                Flow<W>::from_str("Z0*Z4 -> Z0*Z2"),
                Flow<W>::from_str("Z0 -> Z0"),
                Flow<W>::from_str("Z0 -> Z1"),
                Flow<W>::from_str("Z0 -> Z2"),
                Flow<W>::from_str("X0*X2*X4 -> X0*X2*X4"),
                Flow<W>::from_str("X0 -> X0"),
                Flow<W>::from_str("X0 -> Z0"),
            })),
        (std::vector<std::optional<std::vector<int32_t>>>{
            {std::vector<int32_t>{0}},
            {std::vector<int32_t>{1}},
            {std::vector<int32_t>{0, 1}},
            {std::vector<int32_t>{1}},
            {std::vector<int32_t>{}},
            {},
            {std::vector<int32_t>{0}},
            {std::vector<int32_t>{}},
            {},
            {},
        }));
})
