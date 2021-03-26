
namespace cpp fmuproxy.thrift

include "defs.thrift"

service FmuService {

    defs.ModelDescription get_model_description() throws (1: defs.NoSuchFmuException ex)

    void create_instance() throws (1: defs.UnsupportedOperationException ex1, 2: defs.NoSuchFmuException ex2)

    defs.Status setup_experiment(1: double start, 2: double stop, 3: double tolerance) throws (1: defs.NoSuchInstanceException ex)
    defs.Status enter_initialization_mode() throws (1: defs.NoSuchInstanceException ex)
    defs.Status exit_initialization_mode() throws (1: defs.NoSuchInstanceException ex)
    
    defs.StepResult step(1: double stepSize) throws (1: defs.NoSuchInstanceException ex)
    defs.Status reset() throws (1: defs.NoSuchInstanceException ex)
    defs.Status terminate() throws (1: defs.NoSuchInstanceException ex)
    void freeInstance() throws (1: defs.NoSuchInstanceException ex)

    defs.IntegerRead read_integer(1: defs.ValueReferences vr) throws (1: defs.NoSuchInstanceException ex1, 2: defs.NoSuchVariableException ex2)
    defs.RealRead read_real(1: defs.ValueReferences vr) throws (1: defs.NoSuchInstanceException ex1, 2: defs.NoSuchVariableException ex2)
    defs.StringRead read_string(1: defs.ValueReferences vr) throws (1: defs.NoSuchInstanceException ex1, 2: defs.NoSuchVariableException ex2)
    defs.BooleanRead read_boolean(1: defs.ValueReferences vr) throws (1: defs.NoSuchInstanceException ex1, 2: defs.NoSuchVariableException ex2)

    defs.Status write_integer(1: defs.ValueReferences vr, 2: defs.IntArray value) throws (1: defs.NoSuchInstanceException ex1, 2: defs.NoSuchVariableException ex2)
    defs.Status write_real(1: defs.ValueReferences vr, 2: defs.RealArray value) throws (1: defs.NoSuchInstanceException ex1, 2: defs.NoSuchVariableException ex2)
    defs.Status write_string(1: defs.ValueReferences vr, 2: defs.StringArray value) throws (1: defs.NoSuchInstanceException ex1, 2: defs.NoSuchVariableException ex2)
    defs.Status write_boolean(1: defs.ValueReferences vr, 2: defs.BooleanArray value) throws (1: defs.NoSuchInstanceException ex1, 2: defs.NoSuchVariableException ex2)

}
