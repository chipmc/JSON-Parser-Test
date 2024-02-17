#ifndef GATEWAY_H
#define GATEWAY_H

#include "DataReport.h"
#include "MyPersistentData.h" // Where persistent storage data is kept
#include "Particle.h"
#include "MyPersistentData.h"
#include "JsonParserGeneratorRK.h"

class Gateway {
public:
    /**
     * @brief Gets the singleton instance of this class, allocating it if necessary
     * 
     * Use Gatewasy::instance() to instantiate the singleton.
     */
    static Gateway &instance();

    // Function to receive a mock data report and set basic data to MyPersistentData
    void processDataReport(DataReport report);

private:

    // Sets more data to to MyPersistentData
    void decipherDataReport();
  
protected:
    /**
     * @brief The constructor is protected because the class is a singleton
     * 
     * Use Gateway::instance() to instantiate the singleton.
     */
    Gateway();

    /**
     * @brief The destructor is protected because the class is a singleton and cannot be deleted
     */
    virtual ~Gateway();

    /**
     * This class is a singleton and cannot be copied
     */
    Gateway(const Gateway&) = delete;

    /**
     * This class is a singleton and cannot be copied
     */
    Gateway& operator=(const Gateway&) = delete;

    /**
     * @brief Singleton instance of this class
     * 
     * The object pointer to this class is stored here. It's NULL at system boot.
     */
    static Gateway *_instance;
    
};

#endif // GATEWAY_H