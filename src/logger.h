
#ifndef LOGGER_H
#define LOGGER_H

#include "Job.h"
#include <string>

void registrarEvento(const Job& job, const std::string& evento);

#endif
