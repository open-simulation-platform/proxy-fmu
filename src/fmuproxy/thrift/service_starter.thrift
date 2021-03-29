
namespace cpp fmuproxy.thrift

include "defs.thrift"

service FmuServiceStarter {

    void load_fmu_from_local_file(1: string fmu) throws (1: defs.NoSuchFileException ex)
    void load_fmu_from_remote_file(1: string name, 2: binary data)

}
