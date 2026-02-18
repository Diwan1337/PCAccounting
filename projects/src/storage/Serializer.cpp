#include "Serializer.h"
#include <sstream>
#include <stdexcept>

static void writeString(std::ostringstream& out, const std::string& str) {
    size_t size = str.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    out.write(str.c_str(), size);
}

static std::string readString(std::istringstream& in) {
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));

    std::string str(size, '\0');
    in.read(&str[0], size);

    return str;
}

std::vector<unsigned char> Serializer::serialize(const Database& db) {
    std::ostringstream out(std::ios::binary);

    const auto& employees = db.getEmployees();
    size_t employeeCount = employees.size();
    out.write(reinterpret_cast<const char*>(&employeeCount), sizeof(employeeCount));

    for (const auto& e : employees) {
        out.write(reinterpret_cast<const char*>(&e.id), sizeof(e.id));

        writeString(out, e.institute);
        writeString(out, e.department);
        writeString(out, e.lastName);
        writeString(out, e.initials);
        writeString(out, e.position);
        writeString(out, e.phone);
        writeString(out, e.email);
        writeString(out, e.employmentDate);
        writeString(out, e.status);

        bool hasComputer = e.computerId.has_value();
        out.write(reinterpret_cast<const char*>(&hasComputer), sizeof(hasComputer));

        if (hasComputer) {
            int compId = e.computerId.value();
            out.write(reinterpret_cast<const char*>(&compId), sizeof(compId));
        }
    }

    const auto& computers = db.getComputers();
    size_t computerCount = computers.size();
    out.write(reinterpret_cast<const char*>(&computerCount), sizeof(computerCount));

    for (const auto& c : computers) {
        out.write(reinterpret_cast<const char*>(&c.id), sizeof(c.id));

        writeString(out, c.inventoryNumber);
        writeString(out, c.serialNumber);
        writeString(out, c.manufacturer);
        writeString(out, c.model);
        writeString(out, c.cpuModel);
        writeString(out, c.chipset);

        out.write(reinterpret_cast<const char*>(&c.ramSize), sizeof(c.ramSize));
        writeString(out, c.storageType);
        out.write(reinterpret_cast<const char*>(&c.storageSize), sizeof(c.storageSize));

        writeString(out, c.roomNumber);
        writeString(out, c.condition);
        writeString(out, c.commissioningDate);
        writeString(out, c.lastMaintenanceDate);
        writeString(out, c.warrantyExpirationDate);
    }

    std::string buffer = out.str();
    return std::vector<unsigned char>(buffer.begin(), buffer.end());
}

Database Serializer::deserialize(const std::vector<unsigned char>& data) {
    std::istringstream in(
        std::string(data.begin(), data.end()),
        std::ios::binary
    );

    Database db;

    size_t employeeCount;
    in.read(reinterpret_cast<char*>(&employeeCount), sizeof(employeeCount));

    for (size_t i = 0; i < employeeCount; ++i) {
        Employee e;

        in.read(reinterpret_cast<char*>(&e.id), sizeof(e.id));

        e.institute = readString(in);
        e.department = readString(in);
        e.lastName = readString(in);
        e.initials = readString(in);
        e.position = readString(in);
        e.phone = readString(in);
        e.email = readString(in);
        e.employmentDate = readString(in);
        e.status = readString(in);

        bool hasComputer;
        in.read(reinterpret_cast<char*>(&hasComputer), sizeof(hasComputer));

        if (hasComputer) {
            int compId;
            in.read(reinterpret_cast<char*>(&compId), sizeof(compId));
            e.computerId = compId;
        }

        db.addEmployeeWithId(e);
    }

    size_t computerCount;
    in.read(reinterpret_cast<char*>(&computerCount), sizeof(computerCount));

    for (size_t i = 0; i < computerCount; ++i) {
        Computer c;

        in.read(reinterpret_cast<char*>(&c.id), sizeof(c.id));

        c.inventoryNumber = readString(in);
        c.serialNumber = readString(in);
        c.manufacturer = readString(in);
        c.model = readString(in);
        c.cpuModel = readString(in);
        c.chipset = readString(in);

        in.read(reinterpret_cast<char*>(&c.ramSize), sizeof(c.ramSize));
        c.storageType = readString(in);
        in.read(reinterpret_cast<char*>(&c.storageSize), sizeof(c.storageSize));

        c.roomNumber = readString(in);
        c.condition = readString(in);
        c.commissioningDate = readString(in);
        c.lastMaintenanceDate = readString(in);
        c.warrantyExpirationDate = readString(in);

        db.addComputerWithId(c);
    }

    db.validate();
    return db;
}
