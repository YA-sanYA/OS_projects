#include "pch.h"
#include "CppUnitTest.h"
#include "EmployeeService.h"
#include <vector>
#include <string>
#include <filesystem>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EmployeeTests
{
	TEST_CLASS(EmployeeLogicTests)
	{
	public:

		// 1. Тестируем валидацию имени
		TEST_METHOD(TestNameValidation)
		{
			Assert::IsTrue(EmployeeService::isValidName("Ivan"));
			Assert::IsFalse(EmployeeService::isValidName("")); // Пустое нельзя
			Assert::IsFalse(EmployeeService::isValidName("VeryLongName12345")); // Больше 9 нельзя
		}

		// 2. Тестируем валидацию ID и часов
		TEST_METHOD(TestNumericValidation)
		{
			Assert::IsTrue(EmployeeService::isValidID(101));
			Assert::IsFalse(EmployeeService::isValidID(-1));

			Assert::IsTrue(EmployeeService::isValidHours(40.0));
			Assert::IsFalse(EmployeeService::isValidHours(-5.5));
		}

		// 3. Тестируем расчет зарплаты
		TEST_METHOD(TestSalaryCalculation)
		{
			double hours = 10.0;
			double rate = 500.0;
			double expected = 5000.0;

			double actual = EmployeeService::calculateSalary(hours, rate);

			Assert::AreEqual(expected, actual);
		}

		// 4. Интеграционный тест: Запись и чтение файла
		TEST_METHOD(TestFileOperations)
		{
			std::string testFile = "unit_test_data.bin";
			employee outEmp = { 777, "Tester", 15.5 };

			// Удаляем старый файл если есть
			if (std::filesystem::exists(testFile)) {
				std::filesystem::remove(testFile);
			}

			// Сохраняем
			bool saved = EmployeeService::saveEmployee(testFile, outEmp);
			Assert::IsTrue(saved);

			// Читаем обратно
			auto list = EmployeeService::loadAllEmployees(testFile);

			Assert::AreEqual((size_t)1, list.size());
			Assert::AreEqual(outEmp.num, list[0].num);
			Assert::AreEqual(std::string(outEmp.name), std::string(list[0].name));
			Assert::AreEqual(outEmp.hours, list[0].hours);

			// Чистим за собой
			std::filesystem::remove(testFile);
		}
	};
}