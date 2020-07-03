EXAMPLES
------------

Coroutine_context example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is a simple example of using basic coroutine context below:

.. literalinclude:: ../../../sample/sample_readme_1.cpp
    :language: cpp
    :encoding: utf-8

Also, you can use ``copp::coroutine_context_container<ALLOCATOR>`` instead of ``copp::coroutine_context_default`` to use a different stack allocator.

Coroutine task example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is a simple example of using coroutine task with lambda expression:

.. literalinclude:: ../../../sample/sample_readme_2.cpp
    :language: cpp
    :encoding: utf-8

Also, you can your stack allocator or id allocator by setting different parameters in template class **cotask::task<TCO_MACRO>**

Using coroutine task manager
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is a simple example of using task manager:

.. literalinclude:: ../../../sample/sample_readme_3.cpp
    :language: cpp
    :encoding: utf-8

Using stack pool
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is a simple example of using stack pool for cotask:

.. literalinclude:: ../../../sample/sample_readme_4.cpp
    :language: cpp
    :encoding: utf-8

Using then or await_task
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is a simple example of using ``then`` and ``await_task`` for cotask:

.. literalinclude:: ../../../sample/sample_readme_5.cpp
    :language: cpp
    :encoding: utf-8


Using copp::future::future_t and prepare for c++20 coroutine
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is a simple example of using copp::future::future_t<RESULT> and using copp::future::context<RESULT>:

.. literalinclude:: ../../../sample/sample_readme_6.cpp
    :language: cpp
    :encoding: utf-8

Using c++20 coroutine
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. literalinclude:: ../../../sample/sample_readme_7.cpp
    :language: cpp
    :encoding: utf-8

Using c++20 coroutine with custom generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. literalinclude:: ../../../sample/sample_readme_8.cpp
    :language: cpp
    :encoding: utf-8

Custom error (timeout for example) when polling c++20 coroutine task or generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By add context into custom executor or manager and remove it when destroyed, we can add more flexible error handling or procedure.

.. literalinclude:: ../../../sample/sample_readme_9.cpp
    :language: cpp
    :encoding: utf-8

Let c++20 coroutine work with cotask::task<MACRO>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is a simple example to let c++20 coroutine await cotask::task

.. literalinclude:: ../../../sample/sample_readme_10.cpp
    :language: cpp
    :encoding: utf-8


Using ``SetUnhandledExceptionFilter`` on Windows with cotask::task<MACRO>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some applications will use ``SetUnhandledExceptionFilter`` to catch unhandled exception and analysis crash problem. But ``SetUnhandledExceptionFilter`` is only works with `coroutine context of windows fiber <https://github.com/owt5008137/libcopp/blob/v2/include/libcopp/coroutine/coroutine_context_fiber_container.h>`_ . This is a sample of using **windows fiber** as coroutine context in ``cotask::task<MACRO>`` .

.. literalinclude:: ../../../sample/sample_readme_11.cpp
    :language: cpp
    :encoding: utf-8
